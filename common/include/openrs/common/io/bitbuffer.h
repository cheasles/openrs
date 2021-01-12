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

#include <bitset>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

#include "openrs/common/io/buffer.h"

namespace openrs {
namespace common {
namespace io {

template <typename Container = std::vector<uint8_t>>
class BitBuffer : public openrs::common::io::Buffer<Container> {
 private:
  typename Container::size_type bit_position_;

 public:
  BitBuffer() : bit_position_(0), openrs::common::io::Buffer<Container>() {}

  template <typename Type>
  bool PutData(const Type data) {
    this->bit_position_ += sizeof(data);
    return openrs::common::io::Buffer<Container>::PutData(data);
  }

  template <typename Type>
  bool PutData(uint8_t num_bits, const Type& data) {
    auto byte_position = this->position();
    auto bit_offset = 8 - (this->bit_position() & 7);
    this->bit_position_ += num_bits;

    for (; num_bits > bit_offset; bit_offset = 8) {
      if (this->size() <= byte_position) {
        this->resize(byte_position);
      }
      (*this)[byte_position] &= ~((1 << bit_offset) - 1);
      (*this)[byte_position++] |=
          data >> num_bits - bit_offset & ((1 << bit_offset) - 1);
      num_bits -= bit_offset;
    }

    if (this->size() <= byte_position) {
      this->resize(byte_position == 0 ? 1 : byte_position);
    }

    if (num_bits == bit_offset) {
      (*this)[byte_position] &= ~((1 << bit_offset) - 1);
      (*this)[byte_position] |= data & ((1 << bit_offset) - 1);
    } else {
      (*this)[byte_position] &=
          ~(((1 << num_bits) - 1) << bit_offset - num_bits);
      (*this)[byte_position] |= (data & ((1 << num_bits) - 1))
                                << bit_offset - num_bits;
    }
    return true;
  }

  inline void seek(const int& dir,
                   const typename Container::size_type& offset) {
    if (dir == SEEK_SET) {
      this->bit_position_ = offset;
    } else if (dir == SEEK_END) {
      this->bit_position_ = this->size() - offset;
    } else if (dir == SEEK_CUR) {
      this->bit_position_ += offset;
    }
  }

  inline typename Container::size_type bit_position() const {
    return this->bit_position_;
  }

  inline typename Container::size_type position() const {
    return this->bit_position() >> 3;
  }
};

}  // namespace io

}  // namespace common

}  // namespace openrs
