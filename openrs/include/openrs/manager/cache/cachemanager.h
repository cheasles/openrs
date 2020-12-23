#pragma once

#include <fstream>
#include <map>
#include <memory>
#include <string>

#include "openrs/cache/cache.h"
#include "openrs/manager/manager.h"
#include "common/singleton.h"

namespace openrs {

namespace manager {

namespace cache {

class CacheManager : public openrs::manager::Manager,
                     public openrs::common::Singleton<CacheManager> {
 private:
  std::shared_ptr<openrs::cache::Cache> cache_;

  static const std::string kCachePath;

 public:
  bool Init() override;

  bool GetArchiveData(const uint32_t& kStoreId, const uint32_t& kArchiveId,
                      std::vector<uint8_t>* output) const;

  inline const std::shared_ptr<openrs::cache::Cache> cache() const {
    return this->cache_;
  }
};

}  // namespace cache

}  // namespace manager

}  // namespace openrs
