#include "openrs/net/codec/encoder/global/grabencoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

constexpr frozen::map<openrs::net::codec::PacketType, uint8_t, 2>
    openrs::net::codec::encoder::global::GrabEncoder::code_mapping_;

bool openrs::net::codec::encoder::global::GrabEncoder::Encode(
    const openrs::net::codec::Packet& packet,
    openrs::common::io::Buffer<>* buffer) {
  if (!buffer) {
    return false;
  }

  const auto kPacketCode = GrabEncoder::code_mapping_.find(packet.type);
  if (GrabEncoder::code_mapping_.cend() == kPacketCode) {
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
