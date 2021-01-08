/**
 * This file is part of OpenRS.
 *
 * OpenRS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * OpenRS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with OpenRS. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <openrs/common/io/buffer.h>
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
