#pragma once

#include <common/io/buffer.h>
#include <inttypes.h>

#include <array>

namespace openrs {
namespace cache {

class ArchiveReference {
 private:
  /**
   * The identifier of this entry.
   */
  uint32_t name_hash_;

  /**
   * The CRC32 checksum of this entry.
   */
  uint32_t crc_;

  /**
   * The whirlpool digest of this entry.
   */
  std::array<uint8_t, 64> whirlpool_;

  /**
   * The version of this entry.
   */
  uint32_t revision_;

 public:
  ArchiveReference() : name_hash_(0), crc_(0), revision_(0) {}

  inline auto crc() const { return this->crc_; }
  inline auto revision() const { return this->revision_; }
  inline const auto& whirlpool() const { return this->whirlpool_; }
  inline auto name_hash() const { return this->name_hash_; }

  inline void set_crc(const uint32_t& crc) { this->crc_ = crc; }
  inline void set_revision(const uint32_t& revision) {
    this->revision_ = revision;
  }
  inline void set_whirlpool(const std::array<uint8_t, 64>& whirlpool) {
    this->whirlpool_ = whirlpool;
  }
  inline void set_name_hash(const uint32_t& name_hash) {
    this->name_hash_ = name_hash;
  }
};

}  // namespace cache
}  // namespace openrs
