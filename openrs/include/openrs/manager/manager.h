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
#include <openrs/common/singleton.h>

namespace openrs {

namespace manager {

class Manager {
 public:
  virtual bool Init() = 0;
};

}  // namespace manager

}  // namespace openrs
