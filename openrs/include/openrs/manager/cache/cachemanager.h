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

#include <openrs/cache/cache.h>
#include <openrs/common/singleton.h>

#include <map>
#include <memory>
#include <string>

#include "openrs/manager/manager.h"

namespace openrs {

namespace manager {

namespace cache {

class CacheManager : public openrs::manager::Manager,
                     public openrs::common::Singleton<CacheManager> {
 private:
  std::shared_ptr<openrs::cache::Cache> cache_;

  static const std::string kCachePath;

 public:
  bool Init() override;

  bool GetArchiveData(const uint32_t& kStoreId, const uint32_t& kArchiveId,
                      std::vector<uint8_t>* output) const;

  inline const std::shared_ptr<openrs::cache::Cache> cache() const {
    return this->cache_;
  }
};

}  // namespace cache

}  // namespace manager

}  // namespace openrs
