#include "openrs/net/codec/handler/global/worldpackethandler.h"

#include <openrs/common/log.h>

#include "openrs/manager/interfacemanager.h"

void openrs::net::codec::handler::global::WorldPacketHandler::Handle(
    openrs::net::codec::Packet& packet, openrs::net::Session* session) {
  switch (packet.type) {
    case PacketType::kPing: {
      openrs::net::codec::Packet ping_packet;
      ping_packet.type = openrs::net::codec::PacketType::kPing;
      session->Send(ping_packet);
      break;
    }
    case PacketType::kScreen: {
      openrs::game::Player::DisplayMode* display_mode_ptr = nullptr;
      uint16_t* screen_width_ptr = nullptr;
      uint16_t* screen_height_ptr = nullptr;
      uint8_t* switch_screen_mode = nullptr;
      if (!packet.data.GetData(&display_mode_ptr) ||
          !packet.data.GetData(&screen_width_ptr) ||
          !packet.data.GetData(&screen_height_ptr) ||
          !packet.data.GetData(&switch_screen_mode)) {
        common::Log(common::Log::LogLevel::kDebug)
            << "[World] Failed to read packet data.";
        return;
      }
      if (auto player = session->player().lock()) {
        player->set_display_mode(*display_mode_ptr);
        player->set_screen_width(::be16toh(*screen_width_ptr));
        player->set_screen_height(::be16toh(*screen_height_ptr));
        const auto& interface_manager =
            openrs::manager::InterfaceManager::get();
        interface_manager.SendInterfaces(player, session);
      }

      break;
    }
  }
}
