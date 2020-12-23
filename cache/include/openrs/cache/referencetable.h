#pragma once

#include <openrs/common/io/buffer.h>
#include <inttypes.h>

#include <fstream>
#include <string>
#include <vector>

#include "openrs/cache/archivereference.h"

namespace openrs {
namespace cache {

class ReferenceTable {
 public:
  enum Flags {
    kFlagIdentifiers = 0x01,
    kFlagWhirlpool = 0x02,
    kFlagSizes = 0x04,
    kFlagHash = 0x08
  };

 private:
  uint8_t protocol_;
  uint32_t revision_;
  uint8_t flags_;
  std::vector<ArchiveReference> archive_references_;

 public:
  ReferenceTable();
  ReferenceTable(openrs::common::io::Buffer<> data);

  void BuildArchiveReferences(const uint32_t& kLargestArchiveId,
                              const std::vector<uint32_t>& kIds,
                              openrs::common::io::Buffer<>& data);

  inline const auto is_named() const {
    return (this->flags_ & kFlagIdentifiers) != 0;
  }
  inline const auto is_whirlpooled() const {
    return (this->flags_ & kFlagWhirlpool) != 0;
  }
  inline const auto revision() const { return this->revision_; }
};

}  // namespace cache
}  // namespace openrs
