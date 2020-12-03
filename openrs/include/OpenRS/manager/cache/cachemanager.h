#pragma once

#include <fstream>
#include <map>
#include <memory>
#include <string>

#include "Common/singleton.h"

#include "OpenRS/manager/manager.h"
#include "OpenRS/FileSystem/cache.h"

namespace openrs
{

namespace manager
{

namespace cache
{

class CacheManager
    : public openrs::manager::Manager
    , public openrs::common::Singleton<CacheManager>
{
private:
    std::unique_ptr<openrs::filesystem::Cache> cache_;

    static const std::string kCachePath;

public:
    bool Init() override;

    bool GetArchiveData(const uint32_t& kStoreId, const uint32_t& kArchiveId,
        std::vector<uint8_t>* output) const;
};

}  // namespace cache

}  // namespace manager

}  // namespace openrs
