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

#include <openrs/common/io/buffer.h>
#include <stdint.h>

namespace openrs {

namespace net {

namespace codec {

enum struct PacketType : uint8_t {
  kUnknown,
  kHandshake,
  kLogin,
  kLoginWorld,
  kLoginLobby,
  kClientOutdated,
  kStartUp,
  kGrabCache,
  kServerShuttingDown,
  kErrorSession,
  kErrorInvalidLogin,
  kErrorInvalidLoginServer,
  kLoginDetails,
  kInterfaceWindowPane,
  kInterfaceTab,
  kInterfaceComponentAnimation,
  kInterfaceComponentText,
  kInterfaceComponentSetting,
  kInterfaceHide,
  kConfig1,
  kConfig2,
  kConfigGlobal1,
  kConfigGlobal2,
  kConfigFile1,
  kConfigFile2,
  kConfigGameBarPrivate,
  kConfigGameBarOther,
  kMapRegion,
  kDynamicMapRegion,
  kLocalPlayerUpdate,
  kScreen,
  kPing,
  kAFK,
  kFinishedLoadingRegion,
  kReceivedPacketCount,
  kRunEnergy,
  kPlayerOption,
  kItemLook,
  kCustomPacket161,
  kMessage,
  kCreateTile,
  kMusic,
  kMusicEffect,
  kPlayerSkill,
  kActionButton1,
  kActionButton2,
  kActionButton3,
  kActionButton4,
  kActionButton5,
  kActionButton6,
  kActionButton7,
  kActionButton8,
  kActionButton9,
};

enum struct PacketOpCode : uint8_t {
  kNone = 255,
  kUnknown = 0,
};

struct Packet {
  PacketType type;
  openrs::common::io::Buffer<> data;
};

}  // namespace codec

}  // namespace net

}  // namespace openrs
