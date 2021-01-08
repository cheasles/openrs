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
#include "openrs/cache/archive.h"

#include <gzip.h>

#include <stdexcept>

const uint32_t openrs::cache::Archive::kMaxArchiveLength = 0x01000000;

openrs::cache::Archive::Archive(openrs::common::io::Buffer<> data) {
  if (data.size() < 5) {
    throw new std::runtime_error("Invalid cache archive data.");
  }

  this->data_.clear();

  const auto kCompressionHeader = static_cast<Archive::Compression>(data.at(0));
  data.seek(SEEK_SET, sizeof(Archive::Compression));

  uint32_t* compressed_length_ptr = nullptr;
  if (!data.GetData(&compressed_length_ptr) ||
      ::be32toh(*compressed_length_ptr) > kMaxArchiveLength) {
    throw new std::runtime_error("Invalid cache archive data.");
  }

  const uint32_t kCompressedLength = ::be32toh(*compressed_length_ptr);

  if (kCompressedLength > data.size() - data.position()) {
    throw new std::runtime_error("Invalid cache archive data.");
  }

  uint32_t* length_ptr = nullptr;
  switch (kCompressionHeader) {
    case Archive::Compression::NONE:
      this->data_.assign(data.cbegin() + data.position(), data.cend());
      break;
    case Archive::Compression::GZIP: {
      if (!data.GetData(&length_ptr) ||
          ::be32toh(*length_ptr) > kMaxArchiveLength) {
        throw new std::runtime_error("Invalid cache archive data.");
      }
      this->data_.reserve(::be32toh(*length_ptr));
      CryptoPP::StringSource encrypted_source(
          data.data() + data.position(), kCompressedLength, true,
          new CryptoPP::Gunzip(new CryptoPP::VectorSink(this->data_)));
      break;
    }
    default:
      throw new std::runtime_error(
          "Invalid or unsupported cache archive compression.");
  }
}
