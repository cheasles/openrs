#pragma once

#include <endian.h>

#include <fstream>
#include <string>
#include <vector>

namespace openrs {

namespace net {

namespace io {

class Buffer : public std::vector<uint8_t> {
 private:
  std::vector<uint8_t>::size_type position_;

 public:
  Buffer() : position_(0), std::vector<uint8_t>() {}

  template <typename Type>
  bool GetData(Type** output) {
    if (!output) {
      return false;
    }

    if (this->position_ + sizeof(Type) > this->size()) {
      return false;
    }

    *output = reinterpret_cast<Type*>(&this->at(this->position_));
    this->position_ += sizeof(Type);
    return true;
  }

  bool GetString(char** output);
  bool GetString(std::string* output);

  bool GetTriByte(uint32_t* output);

  template <typename Type>
  bool PutData(const Type data) {
    const auto data_ptr = reinterpret_cast<const uint8_t*>(&data);
    this->insert(this->cend(), data_ptr, data_ptr + sizeof(data));
    return true;
  }

  template <typename Type>
  bool PutDataBE(Type data) {
    switch (sizeof(Type)) {
      case sizeof(uint16_t):
        data = ::htobe16(data);
        break;
      case sizeof(uint32_t):
        data = ::htobe32(data);
        break;
      case sizeof(uint64_t):
        data = ::htobe64(data);
        break;
    }

    return this->PutData(data);
  }

  template <typename Type>
  bool PutDataLE(Type data) {
    switch (sizeof(Type)) {
      case sizeof(uint16_t):
        data = ::htole16(data);
        break;
      case sizeof(uint32_t):
        data = ::htole32(data);
        break;
      case sizeof(uint64_t):
        data = ::htole64(data);
        break;
    }

    return this->PutData(data);
  }

  inline void ClearOldData() {
    this->erase(this->cbegin(), this->cbegin() + this->position());
    this->position_ = 0;
  }

  inline void seek(const std::ios_base::seek_dir& dir,
                   const std::vector<uint8_t>::size_type& offset) {
    if (dir == std::ios_base::beg) {
      this->position_ = offset;
    } else if (dir == std::ios_base::end) {
      this->position_ = this->size() - offset;
    } else if (dir == std::ios_base::cur) {
      this->position_ += offset;
    }
  }

  inline std::vector<uint8_t>::size_type position() const {
    return this->position_;
  }
};

}  // namespace io

}  // namespace net

}  // namespace openrs
