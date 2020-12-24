#include "openrs/net/codec/encoder/global/worldencoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

constexpr frozen::map<openrs::net::codec::PacketType, uint8_t, 1>
    openrs::net::codec::encoder::global::WorldEncoder::code_mapping_;

bool openrs::net::codec::encoder::global::WorldEncoder::Encode(
    const openrs::net::codec::Packet& packet,
    openrs::common::io::Buffer<>* buffer) {
  if (!buffer) {
    return false;
  }

  const auto kPacketCode = WorldEncoder::code_mapping_.find(packet.type);
  if (WorldEncoder::code_mapping_.cend() == kPacketCode) {
    return Encoder::Encode(packet, buffer);
  }

  if (static_cast<uint8_t>(PacketOpCode::kNone) != kPacketCode->second) {
    buffer->emplace_back(kPacketCode->second);
  }
  if (packet.data.size() != 0) {
    buffer->insert(buffer->cend(), packet.data.cbegin(), packet.data.cend());
  }

  return true;
}
