#include "OpenRS/manager/cache/cachemanager.h"

#include <limits>
#include <iostream>

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
        std::cerr << ex.what() << std::endl;
        return false;
    }

    return true;
}
