#pragma once

#include <inttypes.h>

#include <fstream>
#include <string>
#include <vector>

namespace openrs
{

namespace filesystem
{

class ReferenceTable
{
public:
    enum Flags
    {
        kFlagIdentifiers = 0x01,
        kFlagWhirlpool = 0x02,
        kFlagSizes = 0x04,
        kFlagHash = 0x08
    };

    struct Entry
    {
        /**
         * The identifier of this entry.
         */
        int identifier;

        /**
         * The CRC32 checksum of this entry.
         */
        int crc;

        /**
         * The compressed size of this entry.
         */
        int compressed;

        /**
         * The uncompressed size of this entry.
         */
        int uncompressed;

        /**
         * The hash of this entry
         */
        int hash;

        /**
         * The whirlpool digest of this entry.
         */
        std::vector<uint8_t> whirlpool;

        /**
         * The version of this entry.
         */
        int version;

        /**
         * The cache index of this entry
         */
        int index;
    };

private:
    uint8_t format_;
    uint32_t version_;
    uint8_t flags_;

public:
    ReferenceTable();
    ReferenceTable(std::ifstream& stream);
};

}  // namespace filesystem

}  // namespace openrs
