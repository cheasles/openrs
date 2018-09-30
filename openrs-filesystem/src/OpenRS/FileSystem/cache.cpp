#include "OpenRS/FileSystem/cache.h"

#include <stdint.h>

#include <limits>
#include <fstream>
#include <iostream>

#include "Common/crc.h"

const uint32_t openrs::filesystem::Cache::kMetadataId = 255;

openrs::filesystem::Cache::Cache(const std::string& path)
{
    auto data_stream = std::make_shared<std::ifstream>();
    data_stream->open(path + "main_file_cache.dat2", std::ios::binary);
    if (!data_stream->is_open())
    {
        throw std::runtime_error("Could not open main data cache.");
    }
    *data_stream >> std::noskipws;
    this->streams_.emplace_back(data_stream);

    for (uint16_t i = 0; i <= std::numeric_limits<uint8_t>::max(); ++i)
    {
        data_stream = std::make_shared<std::ifstream>();
        data_stream->open(path + "main_file_cache.idx" + std::to_string(i),
            std::ios::binary);
        if (data_stream->is_open())
        {
            *data_stream >> std::noskipws;
            this->streams_.emplace_back(data_stream);
            this->indexes_[i] = FileStore(*this->streams_.begin(), data_stream);
        }
    }

    if (this->streams_.size() <= 1)
    {
        throw std::runtime_error("Could not locate any data caches.");
    }

    auto meta_data = this->indexes_.find(kMetadataId);
    if (this->indexes_.cend() == meta_data)
    {
        throw std::runtime_error("Could not locate meta-data index.");
    }

    // Calculate meta-data for the actual cache information.
    for (auto& index : this->indexes_)
    {
        std::vector<uint8_t> index_data;
        if (!meta_data->second.GetArchiveData(index.first, &index_data))
        {
            continue;
        }

        index.second.set_crc(openrs::common::crc32c(0, index_data.data(),
            index_data.size()));
    }
}

