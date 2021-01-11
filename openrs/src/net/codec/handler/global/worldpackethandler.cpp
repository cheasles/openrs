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
#include "openrs/net/codec/handler/global/worldpackethandler.h"

#include <openrs/common/log.h>

#include "openrs/manager/interfacemanager.h"
#include "openrs/manager/worldmanager.h"

openrs::net::codec::handler::global::WorldPacketHandler::WorldPacketHandler()
    : PacketHandler(),
      openrs::common::event::EventSource<
          openrs::event::packet::EventPacketScreen>() {
  this->add_sink(openrs::manager::WorldManager::get());
}

void openrs::net::codec::handler::global::WorldPacketHandler::Handle(
    openrs::net::codec::Packet& packet,
    std::shared_ptr<openrs::net::Session> session) const {
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

      this->EmitEvent({session, *display_mode_ptr, ::be16toh(*screen_width_ptr),
                       ::be16toh(*screen_height_ptr)});
      break;
    }
  }
}
