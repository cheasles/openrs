#pragma once

#include <map>

#include "openrs/cache/filestore.h"
#include "openrs/cache/referencetable.h"

namespace openrs {

namespace cache {

class Container {
 public:
  Container(openrs::cache::FileStore&& store);
};

}  // namespace cache

}  // namespace openrs
