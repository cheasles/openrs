#include "openrs/net/codec/decoder/global/decoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::Decoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  uint8_t* packet_type_ptr = nullptr;
  if (!buffer.GetData(&packet_type_ptr)) {
    return false;
  }

  const auto kPacketType = Decoder::code_mapping_.find(*packet_type_ptr);
  if (Decoder::code_mapping_.cend() == kPacketType) {
    common::Log(common::Log::LogLevel::kWarning)
        << "Invalid packet code: " << std::to_string(*packet_type_ptr);
    return false;
  }

  packet->type = kPacketType->second;
  if (buffer.remaining() > 0) {
    uint8_t* packet_size_ptr = nullptr;
    if (!buffer.GetData(&packet_size_ptr)) {
      return false;
    }

    const auto cbegin = buffer.cbegin() + buffer.position();
    const auto cend = cbegin + *packet_size_ptr;

    if (cbegin >= buffer.cend() || cend > buffer.cend()) {
      return false;
    }

    packet->data.assign(cbegin, cend);
    buffer.seek(SEEK_CUR, packet->data.size());
  }

  common::Log(common::Log::LogLevel::kDebug)
      << "Decoded packet of type "
      << std::to_string(static_cast<uint8_t>(kPacketType->second));

  return true;
}
