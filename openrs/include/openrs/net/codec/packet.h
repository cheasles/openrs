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
  kConfigGlobal1,
  kConfigGlobal2,
  kMapRegion,
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
