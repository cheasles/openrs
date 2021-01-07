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
