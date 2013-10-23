#pragma once

#ifdef LR_DEBUG
#include "MockConf.h"
class tempFileCreate {
public:

   tempFileCreate(MockConf& conf) : mConf(conf) {
      mTestDir << "/tmp/TooMuchPcap." << pthread_self();
   }
   
    
   tempFileCreate(MockConf& conf, const std::string& directory) : mConf(conf) {
      mTestDir << directory << "." << pthread_self();
   }

   ~tempFileCreate() {
      std::string makeADir = "rm -rf ";
      makeADir += mTestDir.str();
      system(makeADir.c_str());
   }

   bool Init() {
      mConf.mPCapCaptureLocations.clear();
      mConf.mPCapCaptureLocations.push_back(mTestDir.str());
      std::string makeADir = "mkdir -p ";
      makeADir += mTestDir.str();
      return system(makeADir.c_str()) == 0;
   }

   std::stringstream mTestDir;
   MockConf& mConf;
};

#endif
