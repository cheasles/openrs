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

#include <memory>

namespace openrs {
namespace common {

template <typename Type>
class Singleton {
 public:
  virtual ~Singleton() {}

  static std::shared_ptr<Type>& get() {
    static std::shared_ptr<Type> object = std::make_shared<Type>();
    return object;
  }
};

}  // namespace common
}  // namespace openrs
