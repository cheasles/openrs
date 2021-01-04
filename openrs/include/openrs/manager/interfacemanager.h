#pragma once

#include <openrs/common/singleton.h>
#include <openrs/game/player.h>

#include <memory>

#include "openrs/manager/manager.h"
#include "openrs/net/session.h"

namespace openrs {
namespace manager {

class InterfaceManager : public openrs::manager::Manager,
                         public openrs::common::Singleton<InterfaceManager> {
 public:
  enum struct PaneID : uint16_t {
    kFixed = 548,
    kResizable = 746,
    kUnknown = 752,
  };
  enum struct PaneType : uint8_t {
    kNone = 0,
  };
  enum struct TabID : uint16_t {
    kOverlay = 1,
    k9 = 9,
    kOverlayFixed = 11,
    kTimerFixed = 11,
    k15 = 15,
    k21 = 21,
    k22 = 22,
    k23 = 23,
    k25 = 25,
    kTimer = 26,
    k37 = 37,
    kCombatStyles = 111,
    kTaskSystem = 112,
    kSkills = 113,
    kQuests = 114,
    kDungeon = 114,
    kInventory = 115,
    kEquipment = 116,
    kPrayerBook = 117,
    kSqueelOfFortune = 119,
    k120 = 120,
    k121 = 121,
    k122 = 122,
    kSettings = 123,
    kEmotes = 124,
    kMusic = 125,
    kNotes = 126,
    kLogout = 129,
    k151 = 151,
    k152 = 152,
    k153 = 153,
    k155 = 155,
    k161 = 161,
    kCombatStylesFixed = 171,
    kTaskSystemFixed = 172,
    kSkillsFixed = 173,
    kQuestsFixed = 174,
    kDungeonFixed = 174,
    kInventoryFixed = 175,
    kEquipmentFixed = 176,
    kPrayerBookFixed = 177,
    kSqueelOfFortuneFixed = 179,
    kFriendsListFixed = 180,
    k181 = 181,
    k182 = 182,
    kSettingsFixed = 183,
    kEmotesFixed = 184,
    kMusicFixed = 185,
    kNotesFixed = 186,
    kLogoutFixed = 189,
    k195 = 195,
    k196 = 196,
    k197 = 197,
    k198 = 198,
  };

  enum struct InterfaceID : uint16_t {
    kNotes = 34,
    k137 = 137,
    k167 = 167,
    kLogout = 182,
    kMusic = 187,
    kSettings = 261,
    kPrayerBook = 271,
    kSkills = 320,
    kEquipment = 387,
    kFriendsList = 550,
    kEmotes = 590,
    kInventory = 679,
    k745 = 745,
    k747 = 747,
    k748 = 748,
    k749 = 749,
    k750 = 750,
    k751 = 751,
    k752 = 752,
    k754 = 754,
    kCombatStyles = 884,
    kDungeon = 939,
    k1109 = 1109,
    k1110 = 1110,
    kSqueelOfFortune = 1139,
    kTaskSystem = 3002,
    kQuests = 3002,
    kTimer = 3039,
  };

  enum struct ComponentID : uint16_t {
    k5 = 5,
    k6 = 6,
    k7 = 7,
    k8 = 8,
    k9 = 9,
    k10 = 10,
    k12 = 12,
  };

  enum struct EmoteID : uint32_t {
    kNotes = 34,
  };

 public:
  InterfaceManager(void) {}

  bool Init() override;

  void SendInterfaces(const std::shared_ptr<openrs::game::Player>& player,
                      openrs::net::Session* session) const;

  inline bool IsResizable(
      const std::shared_ptr<openrs::game::Player>& player) const {
    return player->display_mode() ==
               openrs::game::Player::DisplayMode::kResizable ||
           player->display_mode() ==
               openrs::game::Player::DisplayMode::kUnknown3;
  }

 private:
  void SendFixedInterfaces(const std::shared_ptr<openrs::game::Player>& player,
                           openrs::net::Session* session) const;

  void SendResizableInterfaces(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session) const;

  void SendTab(const std::shared_ptr<openrs::game::Player>& player,
               openrs::net::Session* session, const PaneID& kWindowId,
               const TabID& kTabId, const InterfaceID& kInterfaceId,
               const bool& kNoClipped = true) const;

  void SendInterfaceComponentAnimation(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session, const InterfaceID& kInterfaceId,
      const ComponentID& kComponentId, const EmoteID& kEmote) const;

  void SendInterfaceComponentText(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session, const InterfaceID& kInterfaceId,
      const ComponentID& kComponentId, const std::string& kText) const;

  /**
   * Sends an interface component settings value to the client.
   *
   * @param kPlayer The player to send interface settings to.
   * @param session The client session to send the data to.
   * @param kInterfaceId The interface the setting applies to.
   * @param kComponentId The component the setting applies to.
   * @param kFromSlot
   * @param kToSlot
   * @param kSettingsHash The new settings value.
   */
  void SendInterfaceComponentSettings(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::net::Session* session, const InterfaceID& kInterfaceId,
      const ComponentID& kComponentId, const uint16_t& kFromSlot,
      const uint16_t& kToSlot, const uint32_t& kSettingsHash) const;

  /**
   * Hides (or unhides) an interface component for the client.
   *
   * @param kPlayer The player to send interface hidden values to.
   * @param session The client session to send the data to.
   * @param kInterfaceId The interface the setting applies to.
   * @param kComponentId The component the setting applies to.
   * @param kHidden Whether to hide or unhide the interface component.
   */
  void SendHideInterfaceComponent(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::net::Session* session, const InterfaceID& kInterfaceId,
      const ComponentID& kComponentId, const bool& kHidden) const;

  /**
   * Unlocks specific interface component slots for the client.
   *
   * @param kPlayer The player to send interface settings to.
   * @param session The client session to send the data to.
   * @param kInterfaceId The interface the setting applies to.
   * @param kComponentId The component the setting applies to.
   * @param kFromSlot
   * @param kToSlot
   * @param kSlots The slots to unlock as bit indexes.
   */
  inline void SendUnlockInterfaceComponentOptionsSlot(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::net::Session* session, const InterfaceID& kInterfaceId,
      const ComponentID& kComponentId, const uint16_t& kFromSlot,
      const uint16_t& kToSlot, const std::vector<uint8_t>& kSlots) const {
    uint32_t settings_hash = 0;
    for (const auto& kSlot : kSlots) {
      settings_hash |= 2 << kSlot;
    }
    this->SendInterfaceComponentSettings(kPlayer, session, kInterfaceId,
                                         kComponentId, kFromSlot, kToSlot,
                                         settings_hash);
  }

  inline void SendTab(const std::shared_ptr<openrs::game::Player>& player,
                      openrs::net::Session* session,
                      const TabID& kTabIdResizable, const TabID& kTabIdFixed,
                      const InterfaceID& kInterfaceId) const {
    const auto kWindow =
        this->IsResizable(player) ? PaneID::kResizable : PaneID::kFixed;
    this->SendTab(player, session, kWindow,
                  this->IsResizable(player) ? kTabIdResizable : kTabIdFixed,
                  kInterfaceId);
  }

  /**
   * Sends an overlay interface to the client.
   *
   * @param kPlayer The player to send interface settings to.
   * @param session The client session to send the data to.
   * @param kInterfaceId The interface the setting applies to.
   */
  inline void SendOverlay(const std::shared_ptr<openrs::game::Player>& player,
                          openrs::net::Session* session,
                          const InterfaceID& kInterfaceId) const {
    this->SendTab(player, session, TabID::kOverlay, TabID::kOverlayFixed,
                  kInterfaceId);
  }

  inline void SendTabCombatStyles(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kCombatStyles,
                  TabID::kCombatStylesFixed, InterfaceID::kCombatStyles);
  }

  inline void SendTabTaskSystem(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kTaskSystem, TabID::kTaskSystemFixed,
                  InterfaceID::kTaskSystem);
  }

  inline void SendTabSkills(const std::shared_ptr<openrs::game::Player>& player,
                            openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kSkills, TabID::kSkillsFixed,
                  InterfaceID::kSkills);
  }

  inline void SendTabSettings(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kSettings, TabID::kSettingsFixed,
                  InterfaceID::kSettings);
  }

  inline void SendTabPrayerBook(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kPrayerBook, TabID::kPrayerBookFixed,
                  InterfaceID::kPrayerBook);
  }

  inline void SendTabEmotes(const std::shared_ptr<openrs::game::Player>& player,
                            openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kEmotes, TabID::kEmotesFixed,
                  InterfaceID::kEmotes);
  }

  inline void SendTabQuests(const std::shared_ptr<openrs::game::Player>& player,
                            openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kQuests, TabID::kQuestsFixed,
                  InterfaceID::kQuests);
  }

  inline void SendTabDungeon(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kDungeon, TabID::kDungeonFixed,
                  InterfaceID::kDungeon);
  }

  inline void SendTabInventory(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kInventory, TabID::kInventoryFixed,
                  InterfaceID::kInventory);
  }

  inline void SendTabEquipment(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kEquipment, TabID::kEquipmentFixed,
                  InterfaceID::kEquipment);
  }

  inline void SendTabSqueelOfFortune(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kSqueelOfFortune,
                  TabID::kSqueelOfFortuneFixed, InterfaceID::kSqueelOfFortune);
    this->SendHideInterfaceComponent(
        player, session, InterfaceID::kSqueelOfFortune, ComponentID::k8, true);
    this->SendHideInterfaceComponent(
        player, session, InterfaceID::kSqueelOfFortune, ComponentID::k12, true);
  }

  inline void SendTabTimer(const std::shared_ptr<openrs::game::Player>& player,
                           openrs::net::Session* session) const {
    this->SendTab(player, session, TabID::kTimer, TabID::kTimerFixed,
                  InterfaceID::kTimer);
  }

  /**
   * Unlocks the attack style buttons for a client.
   *
   * @param kPlayer The player to send interface settings to.
   * @param session The client session to send the data to.
   */
  inline void SendUnlockAttackStyleButtons(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::net::Session* session) const {
    this->SendUnlockInterfaceComponentOptionsSlot(kPlayer, session,
                                                  InterfaceID::kCombatStyles,
                                                  ComponentID::k7, -1, 0, {0});
    this->SendUnlockInterfaceComponentOptionsSlot(kPlayer, session,
                                                  InterfaceID::kCombatStyles,
                                                  ComponentID::k8, -1, 0, {0});
    this->SendUnlockInterfaceComponentOptionsSlot(kPlayer, session,
                                                  InterfaceID::kCombatStyles,
                                                  ComponentID::k9, -1, 0, {0});
    this->SendUnlockInterfaceComponentOptionsSlot(kPlayer, session,
                                                  InterfaceID::kCombatStyles,
                                                  ComponentID::k10, -1, 0, {0});
  }

  /**
   * Unlocks the emote buttons for a client.
   *
   * @param kPlayer The player to send interface settings to.
   * @param session The client session to send the data to.
   */
  inline void SendUnlockEmotesBook(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::net::Session* session) const {
    this->SendUnlockInterfaceComponentOptionsSlot(
        kPlayer, session, InterfaceID::kEmotes, ComponentID::k8, 0, 118,
        {0, 1});
  }

  void SendWindowPane(const std::shared_ptr<openrs::game::Player>& player,
                      openrs::net::Session* session, const PaneID& kId,
                      const PaneType& kType) const;
};

}  // namespace manager
}  // namespace openrs
