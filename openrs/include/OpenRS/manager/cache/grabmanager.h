#pragma once

#include <frozen/set.h>
#include <inttypes.h>

#include "Common/singleton.h"
#include "OpenRS/manager/manager.h"
#include "OpenRS/net/io/buffer.h"

namespace openrs {

namespace manager {

namespace cache {

class GrabManager : public openrs::manager::Manager,
                    public openrs::common::Singleton<GrabManager> {
 private:
  static constexpr frozen::set<uint32_t, 26> kGrabKeys = {
      175,   9857,   5907,  4981,  113430, 5558,   0,    2534, 6120,
      52303, 129045, 45253, 64569, 92184,  135106, 3940, 3909, 2447,
      150,   7416,   266,   15,    119620, 153189, 493,  436};

 public:
  inline static void WriteKeysToBuffer(openrs::net::io::Buffer* buffer) {
    for (const auto key : GrabManager::kGrabKeys) {
      buffer->PutDataBE<uint32_t>(key);
    }
  }
};

}  // namespace cache

}  // namespace manager

}  // namespace openrs
