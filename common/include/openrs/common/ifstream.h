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

#include <endian.h>

#include <fstream>

namespace openrs {

namespace common {

namespace ifstream {

inline std::size_t GetStreamSize(std::ifstream& stream) {
  const auto kOffset = stream.tellg();
  stream.seekg(0, std::ios::beg);
  const auto kBegin = stream.tellg();
  stream.seekg(0, std::ios::end);
  const auto kSize = stream.tellg() - kBegin;
  stream.seekg(kOffset, std::ios::beg);
  return kSize;
}

template <typename Type>
inline bool ReadValue(std::ifstream& stream, Type* output) {
  if (!output) {
    return false;
  }

  stream.read(reinterpret_cast<char*>(output), sizeof(Type));
  return stream.gcount() == sizeof(Type);
}

template <typename Type>
inline bool ReadValueBE(std::ifstream& stream, Type* output) {
  if (!ReadValue(stream, output)) {
    return false;
  }

  switch (sizeof(Type)) {
    case sizeof(uint16_t):
      *output = ::be16toh(*output);
      break;
    case sizeof(uint32_t):
      *output = ::be32toh(*output);
      break;
    case sizeof(uint64_t):
      *output = ::be64toh(*output);
      break;
    default:
      return false;
  }

  return true;
}

template <typename Type>
inline bool ReadValueLE(std::ifstream& stream, Type* output) {
  if (!ReadValue(stream, output)) {
    return false;
  }

  switch (sizeof(Type)) {
    case sizeof(uint16_t):
      *output = ::le16toh(*output);
      break;
    case sizeof(uint32_t):
      *output = ::le32toh(*output);
      break;
    case sizeof(uint64_t):
      *output = ::le64toh(*output);
      break;
    default:
      return false;
  }

  return true;
}

}  // namespace ifstream

}  // namespace common

}  // namespace openrs
