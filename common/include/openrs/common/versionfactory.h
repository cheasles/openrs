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

#include <stdint.h>

#include <functional>

namespace openrs {

namespace common {

/**
 * Builds an object based on a specific version.
 */
template <typename Type, typename... Args>
class VersionFactory {
 public:
  typedef uint32_t version;

 private:
  std::map<version, std::function<Type(Args)>> registered_;

 private:
  inline void RegisterBuilder(const version& version,
                              std::function<Type(Args)>& builder) {
    this->registered_[version] = builder;
  }

  inline Type& Build(const version& version, Args... args) const {
    auto builder = this->registered_.find(version);
    if (this->registered_.cend() == builder) {
      throw std::runtime_error(
          "Could not locate builder for specified version.");
    }

    return builder->second(args...);
  }
};

}  // namespace common

}  // namespace openrs
