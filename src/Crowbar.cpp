#include <zmq.h>
#include <zlib.h>
#include <czmq.h>
#include <zframe.h>

#include "Crowbar.h"
#include <boost/thread.hpp>
#include <g3log/g3log.hpp>

/**
 * Construct a crowbar for beating things at the binding location
 * 
 * @param binding
 *   A std::string description of a ZMQ socket
 */
Crowbar::Crowbar(const std::string& binding) :
mBinding(binding), mTip(NULL) {
   
}

/**
 * Construct a crowbar for beating the specific headcrab
 * 
 * @param target
 *   A living(initialized) headcrab
 */
Crowbar::Crowbar(const Headcrab& target) :
mBinding(target.GetBinding()), mTip(NULL) {
   
}

/**
 * Default deconstructor
 */
Crowbar::~Crowbar() {
}

/**
 * Get the high water mark for socket sends
 * 
 * @return 
 *   the high water mark
 */
int Crowbar::GetHighWater() {
   return 1024;
}

/**
 * Get the "tip" socket used to hit things
 * 
 * @return
 *   A pointer to a zmq socket (or NULL in a failure) 
 */
zsock_t* Crowbar::GetTip() {
   zsock_t* tip = zsock_new(ZMQ_REQ);
   if (!tip) {
      return NULL;
   }
   
   zsock_set_sndhwm(tip, GetHighWater());
   zsock_set_rcvhwm(tip, GetHighWater());
   zsock_set_linger(tip, 0);
   int connectRetries = 100;

   while (zsock_connect(tip, mBinding.c_str()) != 0 && connectRetries-- > 0 && !zsys_interrupted) {
      boost::this_thread::interruption_point();
      int err = zmq_errno();
      if (err == ETERM) {
         zsock_destroy(&tip);
         return NULL;
      }
      std::string error(zmq_strerror(err));
      LOG(WARNING) << "Could not connect to " << mBinding << ":" << error;
      zclock_sleep(100);
   }
   if (zsys_interrupted) {
      LOG(INFO) << "Caught Interrupt Signal";
   }
   if (connectRetries <= 0) {
      zsock_destroy(&tip);
      return NULL;
   }

   return tip;
}

bool Crowbar::Wield() {
   if (!mTip) {
      mTip = GetTip();
   }
   
   return (mTip != NULL);
}

bool Crowbar::Swing(const std::string& hit) {
   //std::cout << "sending " << hit << std::endl;
   std::vector<std::string> hits;
   hits.push_back(hit);
   return Flurry(hits);
}

/**
 * Poll to see if the other side of the socket is ready
 * @return 
 */
bool Crowbar::PollForReady() {
   zmq_pollitem_t item;
   if (!mTip) {
      return false;
   }
   item.socket = mTip;
   item.events = ZMQ_POLLOUT;
   int returnVal = zmq_poll(&item, 1, 0);
   if (returnVal < 0) {
      LOG(WARNING) << "Socket error: " << zmq_strerror(zmq_errno());
   }

   return (returnVal >= 1);
}

/**
 * Send a bunch of strings to a socket
 * @param hits
 * @return 
 */
bool Crowbar::Flurry(std::vector<std::string>& hits) {
   if (!mTip) {
      LOG(WARNING) << "Cannot send, not Wielded";
      return false;
   }
   if (!PollForReady()) {
      LOG(WARNING) << "Cannot send, no listener ready";
      return false;
   }
   zmsg_t* message = zmsg_new();
   for (auto it = hits.begin();
           it != hits.end(); it++) {
      zmsg_addmem(message, &((*it)[0]), it->size());
   }
   bool success = true;
   //std::cout << "Sending message with " << zmsg_size(message) << " " << hits.size() << std::endl;
   if (zmsg_send(&message, mTip) != 0) {
      LOG(WARNING) << "zmsg_send returned non-zero exit " << zmq_strerror(zmq_errno());
      success = false;
   }
   if (message) {
      zmsg_destroy(&message);
   }
   return success;
}

bool Crowbar::BlockForKill(std::string& guts) {
   std::vector<std::string> allReplies;
   if (BlockForKill(allReplies) && !allReplies.empty()) {
      guts = allReplies[0];
      return true;
   }
   return false;
}

bool Crowbar::BlockForKill(std::vector<std::string>& guts) {
   if (!mTip) {
      return false;
   }
   zmsg_t* message = zmsg_recv(mTip);
   if (!message) {
      return false;
   }
   guts.clear();
   int msgSize = zmsg_size(message);
   for (int i = 0; i < msgSize; i++) {
      zframe_t* frame = zmsg_pop(message);
      std::string aString;
      aString.insert(0, reinterpret_cast<const char*> (zframe_data(frame)), zframe_size(frame));
      guts.push_back(aString);
      zframe_destroy(&frame);
      //std::cout << guts[0] << " found " << aString << std::endl;
   }


   zmsg_destroy(&message);
   return true;
}

bool Crowbar::WaitForKill(std::string& guts, const int timeout) {
   std::vector<std::string> allReplies;
   if (WaitForKill(allReplies, timeout) && !allReplies.empty()) {
      guts = allReplies[0];
      return true;
   }
   return false;
}

bool Crowbar::WaitForKill(std::vector<std::string>& guts, const int timeout) {
   if (!mTip) {
      return false;
   }

   zpoller_t* poller = zpoller_new(mTip, NULL);
   //if (zsocket_poll(mTip, timeout)) {
   if (zpoller_wait(poller, timeout)) {
      return BlockForKill(guts);
   }
   return false;
}

