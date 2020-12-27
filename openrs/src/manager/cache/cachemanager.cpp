#include "openrs/manager/cache/cachemanager.h"

#include <openrs/cache/cache.h>
#include <openrs/cache/filestore.h>
#include <openrs/common/log.h>

#include <iostream>
#include <limits>

#include "openrs/manager/configmanager.h"
#include "options.h"

const std::string openrs::manager::cache::CacheManager::kCachePath =
    OPENRS_CACHE_DIR "/";

bool openrs::manager::cache::CacheManager::Init() {
  try {
    this->cache_ = std::make_shared<openrs::cache::Cache>(kCachePath);
    openrs::manager::ConfigManager::get().InitCacheConfig(
        718, kCachePath + "config.json");
  } catch (const std::runtime_error& ex) {
    openrs::common::Log(openrs::common::Log::LogLevel::kError)
        << "[CacheManager] " << ex.what();
    return false;
  }

  openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
      << "[CacheManager] Loaded " << this->cache_->GetTypeCount()
      << " archives.";
  return true;
}

bool openrs::manager::cache::CacheManager::GetArchiveData(
    const uint32_t& kIndexId, const uint32_t& kArchiveId,
    std::vector<uint8_t>* output) const {
  if (kIndexId == 255) {
    return this->cache_->main_index().GetArchiveData(kArchiveId, output);
  }
  const auto& store = this->cache_->GetStore(kIndexId);
  return store.GetArchiveData(kArchiveId, output);
}
