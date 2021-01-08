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

#include <vector>

namespace openrs {
namespace cache {

class Archive {
 public:
  enum class Compression : uint8_t { NONE = 0, BZIP2, GZIP };

  static const uint32_t kMaxArchiveLength;

 private:
  openrs::common::io::Buffer<> data_;

 public:
  Archive(openrs::common::io::Buffer<> kData);

  inline auto& data() const { return this->data_; }
};

}  // namespace cache
}  // namespace openrs
