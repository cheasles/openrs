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
#include <openrs/game/player.h>

#include <memory>

#include "openrs/manager/manager.h"

namespace openrs {
namespace manager {

/**
 * Manages data related to the appearance of each player.
 */
class AppearanceManager : public openrs::manager::Manager,
                          public openrs::common::Singleton<AppearanceManager> {
 public:
  /**
   * Body parts that can be coloured or styled.
   */
  enum struct BodyPart : uint8_t {
    kHair,
    kBeard,
    kTorso,
    kLegs,
    kBoots,
    kSkin,
    kArms,
    kForearms,
  };

 public:
  AppearanceManager(void) {}

  /**
   * Creates the required world objects for us to manage.
   *
   * @return Always returns true.
   */
  inline bool Init() override { return true; }

  /**
   * Populate a buffer with information about the appearance of a player.
   *
   * @param kPlayer The player to write appearance data for.
   * @param buffer The buffer object to populate.
   */
  static void GetPlayerAppearance(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::common::io::Buffer<>* buffer);
};

}  // namespace manager
}  // namespace openrs
