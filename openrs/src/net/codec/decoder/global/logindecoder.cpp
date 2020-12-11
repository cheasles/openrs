#include "OpenRS/net/codec/decoder/global/logindecoder.h"

#include <iostream>

#include "OpenRS/net/codec/packet.h"
#include "common/log.h"

constexpr frozen::map<uint8_t, openrs::net::codec::PacketType, 3>
    openrs::net::codec::decoder::global::LoginDecoder::code_mapping_;

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

  return true;
}
