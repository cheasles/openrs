#include "openrs/net/codec/decoder/global/logindecoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::LoginDecoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  if (buffer.size() < 1) {
    return false;
  }

  const auto kPacketType = LoginDecoder::code_mapping_.find(buffer.at(0));
  if (LoginDecoder::code_mapping_.cend() == kPacketType) {
    return Decoder::Decode(buffer, packet);
  }

  packet->type = kPacketType->second;
  if (buffer.size() > 1) {
    packet->data.assign(buffer.cbegin() + sizeof(uint8_t), buffer.cend());
  }

  common::Log(common::Log::LogLevel::kDebug)
      << "[Login] Decoded packet of type "
      << std::to_string(static_cast<uint8_t>(kPacketType->second));

  return true;
}
