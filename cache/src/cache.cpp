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
#include "openrs/cache/cache.h"

#include <stdint.h>

#include <fstream>
#include <iostream>

const uint32_t openrs::cache::Cache::kMetadataId = 255;

openrs::cache::Cache::Cache(const std::string& path) {
  auto data_stream = std::make_shared<std::ifstream>();
  data_stream->open(path + "main_file_cache.dat2", std::ios::binary);
  if (!data_stream->is_open()) {
    throw std::runtime_error("Could not open main data cache.");
  }
  *data_stream >> std::noskipws;
  this->streams_.emplace_back(data_stream);

  data_stream = std::make_shared<std::ifstream>();
  data_stream->open(path + "main_file_cache.idx255", std::ios::binary);
  if (!data_stream->is_open()) {
    throw std::runtime_error("Could not open main index cache.");
  }
  *data_stream >> std::noskipws;
  this->streams_.emplace_back(data_stream);
  this->main_index_ = MainIndex(*this->streams_.begin(), data_stream);

  for (uint16_t i = 0; i <= this->main_index_.archive_count(); ++i) {
    data_stream = std::make_shared<std::ifstream>();
    data_stream->open(path + "main_file_cache.idx" + std::to_string(i),
                      std::ios::binary);
    if (data_stream->is_open()) {
      *data_stream >> std::noskipws;
      this->streams_.emplace_back(data_stream);
      this->indexes_[i] = Index(*this->streams_.begin(), data_stream);
    }
  }

  if (this->streams_.size() <= 1) {
    throw std::runtime_error("Could not locate any data caches.");
  }

  // Calculate meta-data for the actual cache information.
  for (auto& index : this->indexes_) {
    if (!this->main_index_.PopulateIndexMetadata(index.first, index.second)) {
      throw std::runtime_error("Failed to calculate cache index information.");
    }
  }
}
