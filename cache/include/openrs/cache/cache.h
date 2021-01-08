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

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "openrs/cache/index.h"

namespace openrs {

namespace cache {

class Cache {
 private:
  std::list<std::shared_ptr<std::ifstream>> streams_;

  openrs::cache::MainIndex main_index_;
  std::map<uint32_t, openrs::cache::Index> indexes_;

 public:
  static const uint32_t kMetadataId;

 public:
  Cache(const std::string& path);

  inline const openrs::cache::Index& GetStore(const uint32_t& id) const {
    const auto& store = this->indexes_.find(id);
    if (this->indexes_.cend() == store) {
      throw std::runtime_error("Tried to access an unknown store.");
    }

    return store->second;
  }

  inline uint32_t GetTypeCount() const {
    return static_cast<uint32_t>(this->indexes_.size());
  }

  inline const auto& main_index() const { return this->main_index_; }
};

}  // namespace cache

}  // namespace openrs
