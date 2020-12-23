#pragma once

#include <inttypes.h>

#include "common/singleton.h"

namespace openrs {

namespace manager {

class Manager {
 public:
  virtual bool Init() = 0;
};

}  // namespace manager

}  // namespace openrs
