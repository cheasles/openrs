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
    k9 = 9,
    k15 = 15,
    k21 = 21,
    k22 = 22,
    k23 = 23,
    k25 = 25,
    k37 = 37,
    k119 = 119,
    k120 = 120,
    k121 = 121,
    k122 = 122,
    kMusic = 125,
    kNotes = 126,
    kLogout = 129,
    k151 = 151,
    k152 = 152,
    k153 = 153,
    k155 = 155,
    k161 = 161,
    kFriendsListFixed = 180,
    k181 = 181,
    k182 = 182,
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
    kFriendsList = 550,
    k745 = 745,
    k747 = 747,
    k748 = 748,
    k749 = 749,
    k750 = 750,
    k751 = 751,
    k752 = 752,
    k754 = 754,
    k1109 = 1109,
    k1110 = 1110,
    k1139 = 1139,
  };

 public:
  InterfaceManager(void) {}

  bool Init() override;

  void SendInterfaces(const std::shared_ptr<openrs::game::Player>& player,
                      openrs::net::Session* session) const;

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
  void SendWindowPane(const std::shared_ptr<openrs::game::Player>& player,
                      openrs::net::Session* session, const PaneID& kId,
                      const PaneType& kType) const;
};

}  // namespace manager
}  // namespace openrs
