#include "openrs/net/codec/decoder/global/logindecoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::LoginDecoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  uint8_t* packet_type_ptr = nullptr;
  if (!buffer.GetData(&packet_type_ptr)) {
    return false;
  }

  const auto kPacketType = LoginDecoder::code_mapping_.find(*packet_type_ptr);
  if (LoginDecoder::code_mapping_.cend() == kPacketType) {
    buffer.seek(SEEK_CUR, -1 * sizeof(uint8_t));
    return Decoder::Decode(buffer, packet);
  }

  packet->type = kPacketType->second;
  if (buffer.remaining() >= 1) {
    packet->data.assign(buffer.cbegin() + buffer.position(), buffer.cend());
    buffer.seek(SEEK_CUR, packet->data.size());
  }

  common::Log(common::Log::LogLevel::kDebug)
      << "[Login] Decoded packet of type "
      << std::to_string(static_cast<uint8_t>(kPacketType->second));

  return true;
}
