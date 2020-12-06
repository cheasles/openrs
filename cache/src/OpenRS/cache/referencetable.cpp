#include "OpenRS/cache/referencetable.h"

int32_t ReadSmartInt(openrs::common::io::Buffer<> data) {
  char* flag_ptr = nullptr;
  if (!data.GetData(&flag_ptr)) {
    throw std::runtime_error("Failed to read reference table protocol.");
  }

  if (*flag_ptr < 0) {
    int32_t* value_ptr = nullptr;
    if (!data.GetData(&value_ptr)) {
      throw std::runtime_error("Failed to read reference table data.");
    }
    return *value_ptr & 0x7FFFFFFF;
  } else {
    int16_t* value_ptr = nullptr;
    if (!data.GetData(&value_ptr)) {
      throw std::runtime_error("Failed to read reference table data.");
    }
    return *value_ptr;
  }
}

openrs::cache::ReferenceTable::ReferenceTable()
    : protocol_(0), revision_(0), flags_(0) {}

openrs::cache::ReferenceTable::ReferenceTable(
    openrs::common::io::Buffer<> data) {
  uint8_t* protocol_ptr = nullptr;
  uint32_t* revision_ptr = nullptr;
  uint8_t* flags_ptr = nullptr;
  if (!data.GetData(&protocol_ptr)) {
    throw std::runtime_error("Failed to read reference table protocol.");
  }

  this->protocol_ = *protocol_ptr;
  if (this->protocol_ < 5 || this->protocol_ > 7) {
    // Unsupported format.
    throw std::runtime_error("Cache data is not supported.");
  }

  if (this->protocol_ >= 6 && !data.GetData(&revision_ptr)) {
    throw std::runtime_error("Failed to read reference table revision.");
  }

  this->revision_ = ::be32toh(*revision_ptr);

  if (!data.GetData(&flags_ptr)) {
    throw std::runtime_error("Failed to read reference table flags.");
  }

  this->flags_ = *flags_ptr;

  std::vector<int> ids;
  if (this->protocol_ >= 7) {
    int count = ReadSmartInt(data);
    ids.resize(count);
  } else {
    uint16_t* count_ptr = nullptr;
    if (!data.GetData(&count_ptr)) {
      throw std::runtime_error("Failed to read reference table data.");
    }
    ids.resize(::be16toh(*count_ptr));
  }

  int last_archive_id = 0;
  int size = -1;
  for (int i = 0; i < ids.size(); ++i) {
    int delta = 0;
    if (this->protocol_ >= 7) {
      delta = ReadSmartInt(data);
    } else {
      uint16_t* delta_ptr = nullptr;
      if (!data.GetData(&delta_ptr)) {
        throw std::runtime_error("Failed to read reference table data.");
      }
      delta = ::be16toh(*delta_ptr);
    }

    ids[i] = last_archive_id + delta;
    last_archive_id += delta;
    if (ids[i] > size) size = ids[i];
  }
  ++size;
}
