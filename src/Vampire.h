#pragma once
#include <string>
#include <vector>
#include "CZMQToolkit.h"
#include "czmq.h"

class Vampire {
public:
   explicit Vampire(const std::string& location);
   bool PrepareToBeShot();
   std::string GetBinding() const;
   bool GetShot(std::string& wound, const int timeout);
   bool GetStake(void*& stake, const int timeout=1000);
   bool GetStakeNoWait(void*& stake);
   bool GetStakes(std::vector<std::pair<void*, unsigned int> >& stakes,
           const int timeout=1000);
   int GetHighWater();
   void SetHighWater(const int hwm);
   int GetIOThreads();
   void SetIOThreads(const int count);
   void SetOwnSocket(const bool own);
   bool GetOwnSocket();
   virtual ~Vampire();
protected:
   void Destroy();
private:
   void setIpcFilePermissions();
   std::string mLocation;
   int mHwm;
   zsock_t* mBody;
   int mLinger;
   int mIOThredCount;
   bool mOwnSocket;
};
