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
