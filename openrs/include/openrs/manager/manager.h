#pragma once

#include <inttypes.h>
#include <openrs/common/singleton.h>

namespace openrs {

namespace manager {

class Manager {
 public:
  virtual bool Init() = 0;
};

}  // namespace manager

}  // namespace openrs
