#pragma once

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "OpenRS/FileSystem/filestore.h"

namespace openrs
{

namespace filesystem
{

class Cache
{
private:
    std::list<std::shared_ptr<std::ifstream>> streams_;

    std::map<uint32_t, openrs::filesystem::FileStore> indexes_;

    static const uint32_t kMetadataId;

public:
    Cache(const std::string& path);

    inline const openrs::filesystem::FileStore& GetStore(const uint32_t& id) const
    {
        const auto& store = this->indexes_.find(id);
        if (this->indexes_.cend() == store)
        {
            throw std::runtime_error("Tried to access an unknown store.");
        }

        return store->second;
    }

    inline uint32_t GetTypeCount() const
    {
        return static_cast<uint32_t>(this->indexes_.size());
    }
};

}  // namespace filesystem

}  // namespace openrs
