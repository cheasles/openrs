#include "openrs/net/codec/decoder/global/decoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::Decoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  if (buffer.size() < 1) {
    return false;
  }

  const auto kPacketType = Decoder::code_mapping_.find(buffer.at(0));
  if (Decoder::code_mapping_.cend() == kPacketType) {
    common::Log(common::Log::LogLevel::kWarning)
        << "Invalid packet code: " << std::to_string(buffer.at(0));
    return false;
  }

  packet->type = kPacketType->second;
  if (buffer.size() > 1) {
    const uint8_t kSize = buffer.at(1);

    const auto cbegin = buffer.cbegin() + sizeof(uint8_t) + sizeof(uint8_t);
    const auto cend = cbegin + kSize;

    if (cbegin >= buffer.cend() || cend > buffer.cend()) {
      return false;
    }

    packet->data.assign(cbegin, cend);
  }

  common::Log(common::Log::LogLevel::kDebug)
      << "Decoded packet of type "
      << std::to_string(static_cast<uint8_t>(kPacketType->second));

  return true;
}
