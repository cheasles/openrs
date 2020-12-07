#include "OpenRS/cache/referencetable.h"

#include <algorithm>

int32_t ReadSmartInt(openrs::common::io::Buffer<>& data) {
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

  std::vector<uint32_t> ids;
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

  uint32_t last_archive_id = 0;
  uint32_t largest_archive_id = 0;
  for (size_t i = 0; i < ids.size(); ++i) {
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

    ids[i] = last_archive_id += delta;
    if (last_archive_id > largest_archive_id) {
      largest_archive_id = last_archive_id;
    }
  }

  this->BuildArchiveReferences(largest_archive_id, ids, data);
}

void openrs::cache::ReferenceTable::BuildArchiveReferences(
    const uint32_t& kLargestArchiveId, const std::vector<uint32_t>& kIds,
    openrs::common::io::Buffer<>& data) {
  this->archive_references_.clear();
  this->archive_references_.resize(kLargestArchiveId);

  if (this->archive_references_.size() == 0)
  {
    return;
  }

  if (this->is_named()) {
    uint32_t* name_ptr = nullptr;
    for (const auto& kId : kIds) {
      if (!data.GetData(&name_ptr)) {
        throw std::runtime_error("Failed to read reference table data.");
      }
      this->archive_references_[kId].set_name_hash(::be32toh(*name_ptr));
    }
  }

  if (this->is_whirlpooled()) {
    std::array<uint8_t, 64> whirlpool;
    uint32_t* name_ptr = nullptr;
    for (const auto& kId : kIds) {
      if (data.position() + 64 > data.size()) {
        throw std::runtime_error("Failed to read reference table data.");
      }
      std::copy(data.cbegin() + data.position(),
                data.cbegin() + data.position() + 64, whirlpool.begin());
      this->archive_references_[kId].set_whirlpool(whirlpool);
    }
  }

  // Populate CRC hashes:
  uint32_t* crc_ptr = nullptr;
  for (const auto& kId : kIds) {
    if (!data.GetData(&crc_ptr)) {
      throw std::runtime_error("Failed to read reference table data.");
    }
    this->archive_references_[kId].set_crc(::be32toh(*crc_ptr));
  }

  // Populate revision:
  uint32_t* revision_ptr = nullptr;
  for (const auto& kId : kIds) {
    if (!data.GetData(&revision_ptr)) {
      throw std::runtime_error("Failed to read reference table data.");
    }
    this->archive_references_[kId].set_revision(::be32toh(*revision_ptr));
  }
}
