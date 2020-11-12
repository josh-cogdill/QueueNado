/* 
 * Author: Robert Weber
 *
 * Created on November 14, 2012, 2:48 PM
 */
#pragma once

#include <stdint.h>
#include <map>
#include <string>
#include <vector>

class Headcrab {
public:
   explicit Headcrab(const std::string& binding);
   virtual ~Headcrab();
   std::string GetBinding() const;
   bool ComeToLife();

   zsock_t* GetFace();
   bool GetHitBlock(std::vector<std::string>& theHits);
   bool GetHitWait(std::vector<std::string>& theHit,const int timeout);
   bool SendSplatter(std::vector<std::string>& feedback);
   bool GetHitBlock(std::string& theHit);
   bool GetHitWait(std::string& theHit,const int timeout);
   bool SendSplatter(const std::string& feedback);
   static int GetHighWater();
private:

   void setIpcFilePermissions();
   Headcrab(const Headcrab& that) : mFace(NULL) {
   }

   std::string mBinding;
   zsock_t* mFace;
};

