#include "OpenRS/manager/cache/cachemanager.h"

#include <limits>
#include <iostream>

#include "Common/log.h"

#include "OpenRS/FileSystem/filestore.h"

const std::string openrs::manager::cache::CacheManager::kCachePath = "../data/cache/";

bool openrs::manager::cache::CacheManager::Init()
{
    try
    {
        this->cache_ = std::make_unique<filesystem::Cache>(kCachePath);
    }
    catch (const std::runtime_error& ex)
    {
        openrs::common::Log(openrs::common::Log::LogLevel::kError)
            << "[CacheManager] " << ex.what();
        return false;
    }

    openrs::common::Log(openrs::common::Log::LogLevel::kInfo)
        << "[CacheManager] Loaded " << this->cache_->GetTypeCount()
        << " archives.";
    return true;
}

bool openrs::manager::cache::CacheManager::GetArchiveData(const uint32_t& kStoreId,
    const uint32_t& kArchiveId, std::vector<uint8_t>* output) const
{
    const auto& store = this->cache_->GetStore(kStoreId);
    return store.GetArchiveData(kArchiveId, output);
}
