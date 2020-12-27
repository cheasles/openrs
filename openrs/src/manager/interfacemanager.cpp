#include "openrs/manager/interfacemanager.h"

#include <openrs/common/io/buffer.h>
#include <tea.h>

#include "openrs/manager/configmanager.h"

bool openrs::manager::InterfaceManager::Init() { return true; }

void openrs::manager::InterfaceManager::SendInterfaces(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session) const {
  const auto kResizable = this->IsResizable(player);
  const auto kWindow = kResizable ? PaneID::kResizable : PaneID::kFixed;
  this->SendWindowPane(player, session, kWindow, PaneType::kNone);
  if (kResizable) {
    this->SendResizableInterfaces(player, session);
  } else {
    this->SendFixedInterfaces(player, session);
  }
}

void openrs::manager::InterfaceManager::SendFixedInterfaces(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session) const {
  this->SendTab(player, session, PaneID::kFixed, TabID::k161,
                InterfaceID::k752);
  this->SendTab(player, session, PaneID::kFixed, TabID::k37, InterfaceID::k751);
  this->SendTab(player, session, PaneID::kFixed, TabID::k23, InterfaceID::k745);
  this->SendTab(player, session, PaneID::kFixed, TabID::k25, InterfaceID::k754);
  this->SendTab(player, session, PaneID::kFixed, TabID::k155,
                InterfaceID::k747);
  this->SendTab(player, session, PaneID::kFixed, TabID::k151,
                InterfaceID::k748);
  this->SendTab(player, session, PaneID::kFixed, TabID::k152,
                InterfaceID::k749);
  this->SendTab(player, session, PaneID::kFixed, TabID::k153,
                InterfaceID::k750);
  this->SendTab(player, session, PaneID::kUnknown, TabID::k9,
                InterfaceID::k137);
  this->SendTab(player, session, PaneID::kFixed, TabID::k9, InterfaceID::k167);
  // sendMagicBook();
  this->SendTabPrayerBook(player, session);
  this->SendTabEquipment(player, session);
  this->SendTabInventory(player, session);
  this->SendTabQuests(player, session);
  this->SendTabSqueelOfFortune(player, session);
  this->SendTab(player, session, PaneID::kFixed, TabID::k181,
                InterfaceID::k1109);
  this->SendTab(player, session, PaneID::kFixed, TabID::k182,
                InterfaceID::k1110);
  this->SendTab(player, session, PaneID::kFixed, TabID::kFriendsListFixed,
                InterfaceID::kFriendsList);
  this->SendTab(player, session, PaneID::kFixed, TabID::kMusicFixed,
                InterfaceID::kMusic);
  this->SendTab(player, session, PaneID::kFixed, TabID::kNotesFixed,
                InterfaceID::kNotes);
  this->SendTab(player, session, PaneID::kFixed, TabID::kLogoutFixed,
                InterfaceID::kLogout);
  this->SendTabSkills(player, session);
  this->SendTabEmotes(player, session);
  this->SendTabSettings(player, session);
  this->SendTabTaskSystem(player, session);
  this->SendTabCombatStyles(player, session);
  // sendTimerInterface();
}

void openrs::manager::InterfaceManager::SendResizableInterfaces(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session) const {
  this->SendTab(player, session, PaneID::kResizable, TabID::k21,
                InterfaceID::k752);
  this->SendTab(player, session, PaneID::kResizable, TabID::k22,
                InterfaceID::k751);
  this->SendTab(player, session, PaneID::kResizable, TabID::k15,
                InterfaceID::k745);
  this->SendTab(player, session, PaneID::kResizable, TabID::k25,
                InterfaceID::k754);
  this->SendTab(player, session, PaneID::kResizable, TabID::k195,
                InterfaceID::k748);
  this->SendTab(player, session, PaneID::kResizable, TabID::k196,
                InterfaceID::k749);
  this->SendTab(player, session, PaneID::kResizable, TabID::k197,
                InterfaceID::k750);
  this->SendTab(player, session, PaneID::kResizable, TabID::k198,
                InterfaceID::k747);
  this->SendTab(player, session, PaneID::kUnknown, TabID::k9,
                InterfaceID::k137);
  this->SendTabSqueelOfFortune(player, session);
  openrs::manager::ConfigManager::get().SendGlobalConfig(
      player, session, ConfigManager::GlobalConfig::k823, 1);
  this->SendTabCombatStyles(player, session);
  this->SendTabTaskSystem(player, session);
  this->SendTabSkills(player, session);
  this->SendTabQuests(player, session);
  this->SendTabInventory(player, session);
  this->SendTabEquipment(player, session);
  this->SendTabPrayerBook(player, session);
  // sendMagicBook();
  this->SendTab(player, session, PaneID::kResizable, TabID::k120,
                InterfaceID::kFriendsList);
  this->SendTab(player, session, PaneID::kResizable, TabID::k121,
                InterfaceID::k1109);
  this->SendTab(player, session, PaneID::kResizable, TabID::k122,
                InterfaceID::k1110);
  this->SendTabSettings(player, session);
  this->SendTabEmotes(player, session);
  this->SendTab(player, session, PaneID::kResizable, TabID::kMusic,
                InterfaceID::kMusic);
  this->SendTab(player, session, PaneID::kResizable, TabID::kNotes,
                InterfaceID::kNotes);
  this->SendTab(player, session, PaneID::kResizable, TabID::kLogout,
                InterfaceID::kLogout);
}

void openrs::manager::InterfaceManager::SendTab(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session, const PaneID& kWindowId, const TabID& kTabId,
    const InterfaceID& kInterfaceId, const bool& kNoClipped) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutDataBE(
      static_cast<std::underlying_type_t<InterfaceID>>(kInterfaceId));
  // Empty XTEA keys
  buffer.PutData<uint32_t>(0);
  buffer.PutData<uint32_t>(0);
  buffer.PutDataVBE<uint32_t>((static_cast<uint32_t>(kWindowId) << 16) |
                              static_cast<uint32_t>(kTabId));
  buffer.PutData<uint8_t>(kNoClipped ? 1 : 0);
  buffer.PutData<uint32_t>(0);
  buffer.PutData<uint32_t>(0);

  openrs::net::codec::Packet pane_packet;
  pane_packet.type = openrs::net::codec::PacketType::kInterfaceTab;
  pane_packet.data = buffer;
  session->Send(pane_packet);
}

void openrs::manager::InterfaceManager::SendInterfaceComponentText(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session, const InterfaceID& kInterfaceId,
    const ComponentID& kComponentId, const std::string& kText) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutString(kText);
  buffer.PutDataBE<uint32_t>((static_cast<uint32_t>(kInterfaceId) << 16) |
                             static_cast<uint32_t>(kComponentId));

  openrs::net::codec::Packet text_packet;
  text_packet.type = openrs::net::codec::PacketType::kInterfaceComponentText;
  text_packet.data = buffer;
  session->Send(text_packet);
}

void openrs::manager::InterfaceManager::SendInterfaceComponentAnimation(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session, const InterfaceID& kInterfaceId,
    const ComponentID& kComponentId, const EmoteID& kEmote) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutDataVBE<uint32_t>(static_cast<uint32_t>(kEmote));
  buffer.PutDataBE<uint32_t>((static_cast<uint32_t>(kInterfaceId) << 16) |
                             static_cast<uint32_t>(kComponentId));

  openrs::net::codec::Packet animation_packet;
  animation_packet.type =
      openrs::net::codec::PacketType::kInterfaceComponentAnimation;
  animation_packet.data = buffer;
  session->Send(animation_packet);
}

void openrs::manager::InterfaceManager::SendWindowPane(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session, const PaneID& kId,
    const PaneType& kType) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutShiftedNegDataBE(
      static_cast<std::underlying_type_t<PaneType>>(kType));
  buffer.PutShiftedPosDataBE(static_cast<std::underlying_type_t<PaneID>>(kId));
  // Empty XTEA keys
  buffer.resize(buffer.size() + CryptoPP::XTEA::KEYLENGTH);

  openrs::net::codec::Packet pane_packet;
  pane_packet.type = openrs::net::codec::PacketType::kInterfaceWindowPane;
  pane_packet.data = buffer;
  session->Send(pane_packet);
}
