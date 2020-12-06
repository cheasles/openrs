#pragma once

#include <inttypes.h>

#include <array>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Common/ifstream.h"

#include "OpenRS/cache/referencetable.h"

namespace openrs
{

namespace cache
{

class FileStore
{
private:
    std::weak_ptr<std::ifstream> data_stream_;
    std::weak_ptr<std::ifstream> index_stream_;

    uint32_t crc_;
    openrs::cache::ReferenceTable reference_table_;
    std::array<uint8_t, 64> whirlpool_;

    static const uint32_t kIdxBlockLength;
    static const uint32_t kHeaderLength;
    static const uint32_t kHeaderExpandedLength;
    static const uint32_t kBlockLength;
    static const uint32_t kBlockExpandedLength;
    static const uint32_t kTotalBlockLength;

public:
    FileStore() = default;
    FileStore(const std::weak_ptr<std::ifstream>& kDataStream,
        const std::weak_ptr<std::ifstream>& kIndexStream);

    FileStore& operator=(FileStore&&) = default;

    bool GetArchiveData(const int& kArchiveId,
        std::vector<uint8_t>* output) const;

    inline uint32_t crc() const
    {
        return this->crc_;
    }

    inline const openrs::cache::ReferenceTable& reference_table() const
    {
        return this->reference_table_;
    }

    inline const std::array<uint8_t, 64>& whirlpool() const
    {
        return this->whirlpool_;
    }

    inline void set_crc(const uint32_t& crc)
    {
        this->crc_ = crc;
    }

    inline void set_whirlpool(const std::array<uint8_t, 64>& whirlpool)
    {
        this->whirlpool_ = whirlpool;
    }

    static inline uint32_t GetCompressedUInt32(std::ifstream& stream)
    {
        uint32_t value = 0;
        uint8_t temp = 0;
        if (!openrs::common::ifstream::ReadValue(stream, &temp))
        {
            return 0;
        }

        value |= temp << 16;

        if (!openrs::common::ifstream::ReadValue(stream, &temp))
        {
            return 0;
        }

        value |= temp << 8;

        if (!openrs::common::ifstream::ReadValue(stream, &temp))
        {
            return 0;
        }

        value |= temp;
        return value;
    }
};

}  // namespace cache

}  // namespace openrs

