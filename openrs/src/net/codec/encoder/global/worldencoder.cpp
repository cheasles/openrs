#include "openrs/net/codec/encoder/global/worldencoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::encoder::global::WorldEncoder::Encode(
    const openrs::net::codec::Packet& packet,
    const std::weak_ptr<openrs::game::Player>& player,
    openrs::common::io::Buffer<>* buffer) {
  if (!buffer) {
    return false;
  }

  const auto kPacketCode = WorldEncoder::code_mapping_.find(packet.type);
  if (WorldEncoder::code_mapping_.cend() == kPacketCode) {
    return Encoder::Encode(packet, player, buffer);
  }

  const auto kPacketType = WorldEncoder::type_mapping_.find(packet.type);
  if (static_cast<uint8_t>(PacketOpCode::kNone) != kPacketCode->second) {
    buffer->emplace_back(kPacketCode->second);
  }
  switch (kPacketType->second) {
    case PacketHeaderType::kUint16:
      buffer->PutDataBE<uint16_t>(packet.data.size());
      break;
    case PacketHeaderType::kUint8:
      buffer->PutDataBE<uint8_t>(packet.data.size());
      break;
    default:
      break;
  }

  if (packet.data.size() != 0) {
    buffer->insert(buffer->cend(), packet.data.cbegin(), packet.data.cend());
  }

  return true;
}
