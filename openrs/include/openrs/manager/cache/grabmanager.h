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

#include <inttypes.h>
#include <openrs/common/io/buffer.h>
#include <openrs/common/singleton.h>

#include "openrs/manager/configmanager.h"
#include "openrs/manager/manager.h"

namespace openrs {

namespace manager {

namespace cache {

class GrabManager : public openrs::manager::Manager,
                    public openrs::common::Singleton<GrabManager> {
 public:
  inline static void WriteKeysToBuffer(openrs::common::io::Buffer<>* buffer) {
    const auto& grab_config = openrs::manager::ConfigManager::get()["grab"];
    for (const auto key : grab_config["keys"].get<std::vector<uint32_t>>()) {
      buffer->PutDataBE<uint32_t>(key);
    }
  }
};

}  // namespace cache

}  // namespace manager

}  // namespace openrs
