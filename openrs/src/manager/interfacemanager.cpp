#include "openrs/manager/interfacemanager.h"

#include <openrs/common/io/buffer.h>
#include <tea.h>

bool openrs::manager::InterfaceManager::Init() { return true; }

void openrs::manager::InterfaceManager::SendInterfaces(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session) const {
  const auto kResizable =
      player->display_mode() == openrs::game::Player::DisplayMode::kUnknown2 ||
      player->display_mode() == openrs::game::Player::DisplayMode::kUnknown3;
  this->SendWindowPane(player, session,
                       kResizable ? PaneID::kResizable : PaneID::kFixed,
                       PaneType::kNone);
}

void openrs::manager::InterfaceManager::SendWindowPane(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session, const PaneID& kId,
    const PaneType& kType) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutShiftedNegDataBE(static_cast<std::underlying_type_t<PaneType>>(kType));
  buffer.PutShiftedPosDataBE(static_cast<std::underlying_type_t<PaneID>>(kId));
  // Empty XTEA keys
  buffer.resize(buffer.size() + CryptoPP::XTEA::KEYLENGTH);

  openrs::net::codec::Packet pane_packet;
  pane_packet.type = openrs::net::codec::PacketType::kWindowPane;
  pane_packet.data = buffer;
  session->Send(pane_packet);
}
