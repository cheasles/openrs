#pragma once

#include <endian.h>

#include <fstream>
#include <limits>
#include <string>
#include <vector>

namespace openrs {

namespace common {

namespace io {

template <typename Container = std::vector<uint8_t>>
class Buffer : public Container {
 private:
  typename Container::size_type position_;

 public:
  Buffer() : position_(0), Container() {}

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

  bool GetString(char** output) {
    if (!output) {
      return false;
    }

    if (!this->GetData<char>(output)) {
      return false;
    }

    char* index = *output;
    while (index && *index != '\n' && *index != 0) {
      if (!this->GetData<char>(&index)) {
        return false;
      }
    }

    return true;
  }

  bool GetString(std::string* output) {
    if (!output) {
      return false;
    }

    char* c_str = nullptr;
    if (!this->GetString(&c_str)) {
      return false;
    }

    output->assign(c_str);
    return true;
  }

  bool GetTriByte(uint32_t* output) {
    if (!output) {
      return false;
    }

    struct TriByte {
      uint8_t bytes[3];
    };

    TriByte* tribyte;
    if (!this->GetData<TriByte>(&tribyte)) {
      return false;
    }

    *output = (tribyte->bytes[0] << 16) + (tribyte->bytes[1] << 8) +
              (tribyte->bytes[2]);
    return true;
  }

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

  bool PutString(char* input) {
    char index = *input;
    while (index != '\n' && index != 0) {
      if (!this->PutData<char>(index)) {
        return false;
      }
    }

    return this->PutData<char>(0);
  }

  bool PutString(const std::string& kInput) {
    for (auto it = kInput.cbegin(); it != kInput.cend(); ++it) {
      if (!this->PutData(*it)) {
        return false;
      }
    }
    return this->PutData<char>(0);
  }

  template <typename Type>
  inline Type ShiftNegative(const Type& kValue, const uint8_t& kShift) const {
    return (kValue & ~std::numeric_limits<uint8_t>::max()) +
           (kShift -
            static_cast<uint8_t>(kValue & std::numeric_limits<uint8_t>::max()));
  }

  template <typename Type>
  inline Type ShiftPositive(const Type& kValue, const uint8_t& kShift) const {
    return (kValue & ~std::numeric_limits<uint8_t>::max()) +
           static_cast<uint8_t>(
               kShift + static_cast<uint8_t>(
                            kValue & std::numeric_limits<uint8_t>::max()));
  }

  template <typename Type>
  inline bool PutShiftedNegDataBE(const Type& kValue,
                                  const uint8_t& kShift = 128) {
    return this->PutDataBE(this->ShiftNegative(kValue, kShift));
  }

  template <typename Type>
  inline bool PutShiftedNegDataLE(const Type& kValue,
                                  const uint8_t& kShift = 128) {
    return this->PutDataLE(this->ShiftNegative(kValue, kShift));
  }

  template <typename Type>
  inline bool PutShiftedPosDataBE(const Type& kValue,
                                  const uint8_t& kShift = 128) {
    return this->PutDataBE(this->ShiftPositive(kValue, kShift));
  }

  template <typename Type>
  inline bool PutShiftedPosDataLE(const Type& kValue,
                                  const uint8_t& kShift = 128) {
    return this->PutDataLE(this->ShiftPositive(kValue, kShift));
  }

  template <typename Type>
  inline bool PutDataVBE(const Type& kValue) {
    static_assert(sizeof(Type) % 2 == 0);
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&kValue);
    for (int i = 0; i < sizeof(Type); i += 2) {
      if (!this->PutData(p[i + 1]) || !this->PutData(p[i])) {
        return false;
      }
    }

    return true;
  }

  template <typename Type>
  inline bool PutDataVLE(const Type& kValue) {
    static_assert(sizeof(Type) % 2 == 0);
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&kValue);
    for (int i = sizeof(Type); i != 0; i -= 2) {
      if (!this->PutData(p[i - 2]) || !this->PutData(p[i - 1])) {
        return false;
      }
    }

    return true;
  }

  inline void ClearOldData() {
    this->erase(this->cbegin(), this->cbegin() + this->position());
    this->position_ = 0;
  }

  inline void seek(const int& dir,
                   const std::vector<uint8_t>::size_type& offset) {
    if (dir == SEEK_SET) {
      this->position_ = offset;
    } else if (dir == SEEK_END) {
      this->position_ = this->size() - offset;
    } else if (dir == SEEK_CUR) {
      this->position_ += offset;
    }
  }

  inline typename Container::size_type position() const {
    return this->position_;
  }

  inline typename Container::size_type remaining() const {
    return this->size() - this->position_;
  }
};

}  // namespace io

}  // namespace common

}  // namespace openrs
