#pragma once

#include <common/io/buffer.h>
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
