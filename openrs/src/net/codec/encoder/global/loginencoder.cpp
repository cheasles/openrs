#include "openrs/net/codec/encoder/global/loginencoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::encoder::global::LoginEncoder::Encode(
    const openrs::net::codec::Packet& packet,
    const std::weak_ptr<openrs::game::Player>& player,
    openrs::common::io::Buffer<>* buffer) {
  if (!buffer) {
    return false;
  }

  const auto kPacketCode = LoginEncoder::code_mapping_.find(packet.type);
  if (LoginEncoder::code_mapping_.cend() == kPacketCode) {
    return Encoder::Encode(packet, player, buffer);
  }

  if (static_cast<uint8_t>(PacketOpCode::kNone) != kPacketCode->second) {
    buffer->emplace_back(kPacketCode->second);
  }
  if (packet.data.size() != 0) {
    buffer->insert(buffer->cend(), packet.data.cbegin(), packet.data.cend());
  }

  common::Log(common::Log::LogLevel::kDebug)
      << "[Login] Encoded packet of type "
      << std::to_string(kPacketCode->second);

  return true;
}
