#pragma once

#include <inttypes.h>
#include <openrs/common/io/buffer.h>
#include <openrs/common/singleton.h>

#include "openrs/manager/configmanager.h"
#include "openrs/manager/manager.h"

namespace openrs {

namespace manager {

namespace cache {

class GrabManager : public openrs::manager::Manager,
                    public openrs::common::Singleton<GrabManager> {
 public:
  inline static void WriteKeysToBuffer(openrs::common::io::Buffer<>* buffer) {
    const auto& grab_config = openrs::manager::ConfigManager::get()["grab"];
    for (const auto key : grab_config["keys"].get<std::vector<uint32_t>>()) {
      buffer->PutDataBE<uint32_t>(key);
    }
  }
};

}  // namespace cache

}  // namespace manager

}  // namespace openrs
