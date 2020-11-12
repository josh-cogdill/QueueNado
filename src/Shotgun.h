#pragma once


#include <stdlib.h>
#include "czmq.h"
#include <vector>
#include <string>
class Shotgun {
public:
   Shotgun();
   void Aim(const std::string& location);
   void Fire(const std::string& msg);
   void Fire(const std::vector<std::string>& bullets);
   virtual ~Shotgun();
private:
   void setIpcFilePermissions(const std::string& location);
   zsock_t* mGun;
};
