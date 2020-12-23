#pragma once

#include <inttypes.h>
#include <openrs/common/ifstream.h>

#include <array>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "openrs/cache/referencetable.h"

namespace openrs {
namespace cache {

class Index {
 protected:
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
  Index() = default;
  Index(const std::weak_ptr<std::ifstream>& kDataStream,
        const std::weak_ptr<std::ifstream>& kIndexStream);

  Index& operator=(Index&&) = default;

  bool GetArchiveData(const int& kArchiveId,
                      std::vector<uint8_t>* output) const;

  inline uint32_t crc() const { return this->crc_; }

  inline const auto& reference_table() const { return this->reference_table_; }

  inline const std::array<uint8_t, 64>& whirlpool() const {
    return this->whirlpool_;
  }

  inline void set_crc(const uint32_t& crc) { this->crc_ = crc; }

  inline void set_whirlpool(const std::array<uint8_t, 64>& whirlpool) {
    this->whirlpool_ = whirlpool;
  }

  inline void set_reference_table(
      const openrs::cache::ReferenceTable& reference_table) {
    this->reference_table_ = reference_table;
  }

  static inline uint32_t GetCompressedUInt32(std::ifstream& stream) {
    uint32_t value = 0;
    uint8_t temp = 0;
    if (!openrs::common::ifstream::ReadValue(stream, &temp)) {
      return 0;
    }

    value |= temp << 16;

    if (!openrs::common::ifstream::ReadValue(stream, &temp)) {
      return 0;
    }

    value |= temp << 8;

    if (!openrs::common::ifstream::ReadValue(stream, &temp)) {
      return 0;
    }

    value |= temp;
    return value;
  }
};

class MainIndex : public Index {
 public:
  MainIndex() : Index() {}
  MainIndex(const std::weak_ptr<std::ifstream>& kDataStream,
            const std::weak_ptr<std::ifstream>& kIndexStream)
      : Index(kDataStream, kIndexStream) {}

  const uint32_t archive_count() const {
    auto index_stream = this->index_stream_.lock();
    if (!index_stream || index_stream->bad()) {
      return 0;
    }

    return common::ifstream::GetStreamSize(*index_stream) / 6;
  }

  /* @brief Populate hash information for an Index
   */
  bool PopulateIndexMetadata(const uint32_t& kIndexId, Index& index);
};

}  // namespace cache
}  // namespace openrs
