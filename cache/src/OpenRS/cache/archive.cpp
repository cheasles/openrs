#include "OpenRS/cache/archive.h"

#include <gzip.h>

#include <stdexcept>

const uint32_t openrs::cache::Archive::kMaxArchiveLength = 0x01000000;

openrs::cache::Archive::Archive(openrs::common::io::Buffer<> data) {
  if (data.size() < 5) {
    throw new std::runtime_error("Invalid cache archive data.");
  }

  this->data_.clear();

  const auto kCompressionHeader = static_cast<Archive::Compression>(data.at(0));
  data.seek(std::ios_base::beg, sizeof(Archive::Compression));

  uint32_t* compressed_length_ptr = nullptr;
  if (!data.GetData(&compressed_length_ptr) ||
      ::be32toh(*compressed_length_ptr) > kMaxArchiveLength) {
    throw new std::runtime_error("Invalid cache archive data.");
  }

  const uint32_t kCompressedLength = ::be32toh(*compressed_length_ptr);

  if (kCompressedLength > data.size() - data.position()) {
    throw new std::runtime_error("Invalid cache archive data.");
  }

  CryptoPP::Gunzip gzip;
  switch (kCompressionHeader) {
    case Archive::Compression::NONE:
      this->data_.assign(data.cbegin() + data.position(), data.cend());
      break;
    case Archive::Compression::GZIP:
      gzip.Put(data.data() + data.position(), kCompressedLength);
      gzip.MessageEnd();
      this->data_.resize(gzip.MaxRetrievable());
      gzip.Get(this->data_.data(), this->data_.size());
      break;
    default:
      throw new std::runtime_error(
          "Invalid or unsupported cache archive compression.");
  }

  this->data_.assign(data.cbegin() + data.position(), data.cend());
}
