/* 
 * File:   CZMQToolkitTests.h
 * Author: root
 *
 * Created on November 16, 2012, 10:39 AM
 */

#pragma once

#include "gtest/gtest.h"
#include <sys/time.h>
#include <sstream>
#include <string>
#include "CZMQToolkit.h"
#include <czmq.h>
#include <boost/thread.hpp>

class CZMQToolkitTests : public ::testing::Test {
public:

   CZMQToolkitTests() : mReplySocket(nullptr), mRequestSocket(nullptr) {
   }

   virtual ~CZMQToolkitTests() {
   }

protected:

   virtual void SetUp() {
      std::stringstream makeATarget;
      makeATarget << "ipc:///tmp/ipc.test" << boost::this_thread::get_id();
      mTarget = makeATarget.str();
      srandom((unsigned) time(nullptr));
      zsys_interrupted = false;

      mReplySocket = zsock_new(ZMQ_REP);
      ASSERT_NE(nullptr, mReplySocket);
      zsock_set_hwm(mReplySocket, 1);
      zsock_set_linger(mReplySocket, 0);
      int bindRetries = 100;
      while ((zsock_bind(mReplySocket, mTarget.c_str()) < 0) && bindRetries-- > 0) {
         zclock_sleep(100);
      }
      ASSERT_LT(0, bindRetries);

      mRequestSocket = zsock_new(ZMQ_REQ);
      ASSERT_NE(nullptr, mRequestSocket);
      zsock_set_hwm(mRequestSocket, 1);
      zsock_set_linger(mRequestSocket, 0);
      int connectRetries = 100;
      while ((zsock_connect(mRequestSocket, mTarget.c_str()) < 0) && connectRetries-- > 0) {
         zclock_sleep(100);
      }
      ASSERT_LT(0, connectRetries);
   }

   virtual void TearDown() {
      raise(SIGTERM);
      ASSERT_EQ(0, zsock_unbind(mReplySocket, mTarget.c_str()));
      mReplySocket = nullptr;
      ASSERT_EQ(0, zsock_disconnect(mRequestSocket, mTarget.c_str()));
      mRequestSocket = nullptr;
   }

   std::string mTarget;
   zsock_t* mReplySocket;
   zsock_t* mRequestSocket;
};
