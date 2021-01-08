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
#include "openrs/cache/index.h"

#include <openrs/common/crc.h>
#include <openrs/common/ifstream.h>
#include <whrlpool.h>

#include <iterator>

#include "openrs/cache/archive.h"

const uint32_t openrs::cache::Index::kIdxBlockLength = 6;
const uint32_t openrs::cache::Index::kHeaderLength = 8;
const uint32_t openrs::cache::Index::kHeaderExpandedLength = 10;
const uint32_t openrs::cache::Index::kBlockLength = 512;
const uint32_t openrs::cache::Index::kBlockExpandedLength = 510;
const uint32_t openrs::cache::Index::kTotalBlockLength =
    kHeaderLength + kBlockLength;

openrs::cache::Index::Index(const std::weak_ptr<std::ifstream>& kDataStream,
                            const std::weak_ptr<std::ifstream>& kIndexStream)
    : data_stream_(kDataStream), index_stream_(kIndexStream) {}

bool openrs::cache::Index::GetArchiveData(const int& kArchiveId,
                                          std::vector<uint8_t>* output) const {
  auto index_stream = this->index_stream_.lock();
  if (!index_stream || index_stream->bad()) {
    return false;
  }

  auto data_stream = this->data_stream_.lock();
  if (!data_stream || data_stream->bad()) {
    return false;
  }

  index_stream->clear();
  data_stream->clear();

  if (common::ifstream::GetStreamSize(*index_stream) <
      (kArchiveId + 1) * kIdxBlockLength) {
    return false;
  }

  // Use the meta-data index to determine the archive size.
  index_stream->seekg(kArchiveId * kIdxBlockLength, std::ios::beg);

  const uint32_t kSize = Index::GetCompressedUInt32(*index_stream);
  uint32_t block = Index::GetCompressedUInt32(*index_stream);

  const uint32_t kDataBlockCount =
      common::ifstream::GetStreamSize(*data_stream) / kTotalBlockLength;

  if (block == 0 || block > kDataBlockCount) {
    return false;
  }

  output->reserve(kSize);
  uint32_t remaining = kSize;
  uint16_t chunk = 0;
  const uint32_t kBlockLength =
      kArchiveId <= 0xFFFF ? Index::kBlockLength : Index::kBlockExpandedLength;
  const uint32_t kHeaderLength = kArchiveId <= 0xFFFF
                                     ? Index::kHeaderLength
                                     : Index::kHeaderExpandedLength;

  while (remaining > 0) {
    if (block == 0) {
      return false;
    }

    const uint32_t kBlockSize =
        remaining > kBlockLength ? kBlockLength : remaining;
    data_stream->seekg(block * kTotalBlockLength, std::ios::beg);

    // Read in the chunk header.
    uint32_t current_file = 0;
    if (kArchiveId <= 0xFFFF) {
      uint16_t temp_current_file = 0;
      if (!common::ifstream::ReadValueBE(*data_stream, &temp_current_file)) {
        return false;
      }
      current_file = temp_current_file;
    } else {
      if (!common::ifstream::ReadValueBE(*data_stream, &current_file)) {
        return false;
      }
    }

    uint16_t current_chunk = 0;
    if (!common::ifstream::ReadValueBE(*data_stream, &current_chunk)) {
      return false;
    }
    uint32_t next_block = Index::GetCompressedUInt32(*data_stream);
    uint8_t current_index = 0;
    if (!common::ifstream::ReadValue(*data_stream, &current_index)) {
      return false;
    }

    // Validate the header info against what we think we're reading.
    if (kArchiveId != current_file || chunk != current_chunk) {
      return false;
    }

    // Now read the chunk data.
    const auto kOffset = output->size();
    output->resize(kOffset + kBlockSize);
    data_stream->read(reinterpret_cast<char*>(&output->at(kOffset)),
                      kBlockSize);

    // Configure the next chunk.
    remaining -= kBlockSize;
    block = next_block;
    ++chunk;
  }

  return true;
}

bool openrs::cache::MainIndex::PopulateIndexMetadata(const uint32_t& kIndexId,
                                                     Index& index) {
  openrs::common::io::Buffer<> index_data;
  if (!this->GetArchiveData(kIndexId, &index_data)) {
    return false;
  }

  index.set_crc(
      openrs::common::crc32c(0, index_data.data(), index_data.size()));
  std::array<uint8_t, 64> whirlpool;
  CryptoPP::Whirlpool().CalculateDigest(whirlpool.data(), index_data.data(),
                                        index_data.size());
  index.set_whirlpool(whirlpool);

  cache::Archive archive(index_data);
  index.set_reference_table(std::move(cache::ReferenceTable(archive.data())));
  return true;
}
