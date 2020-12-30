#include "openrs/net/codec/decoder/global/worlddecoder.h"

#include <endian.h>
#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::WorldDecoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  uint8_t* packet_type_ptr = nullptr;
  if (!buffer.GetData(&packet_type_ptr)) {
    return false;
  }

  const auto kPacketType = WorldDecoder::code_mapping_.find(*packet_type_ptr);
  if (WorldDecoder::code_mapping_.cend() == kPacketType) {
    buffer.seek(SEEK_CUR, -1 * sizeof(uint8_t));
    return Decoder::Decode(buffer, packet);
  }

  const auto kPacketSize =
      WorldDecoder::packet_sizes_.find(kPacketType->second);
  if (WorldDecoder::packet_sizes_.cend() == kPacketSize) {
    common::Log(common::Log::LogLevel::kWarning)
        << "[World] Received packet without size specification: "
        << std::to_string(static_cast<uint8_t>(kPacketType->second));
    return false;
  }

  packet->type = kPacketType->second;
  switch (kPacketSize->second) {
    case -1: {
      uint8_t* length_ptr = nullptr;
      if (!buffer.GetData(&length_ptr)) {
        common::Log(common::Log::LogLevel::kDebug)
            << "[World] Received packet with missing size.";
        return false;
      }
      packet->data.assign(
          buffer.cbegin() + buffer.position(),
          buffer.cbegin() + buffer.position() +
              std::min(static_cast<size_t>(*length_ptr), buffer.remaining()));
    }
    case -2: {
      uint16_t* length_ptr = nullptr;
      if (!buffer.GetData(&length_ptr)) {
        common::Log(common::Log::LogLevel::kDebug)
            << "[World] Received packet with missing size.";
        return false;
      }
      packet->data.assign(
          buffer.cbegin() + buffer.position(),
          buffer.cbegin() + buffer.position() +
              std::min(static_cast<size_t>(::be16toh(*length_ptr)),
                       buffer.remaining()));
    }
    case -3: {
      uint32_t* length_ptr = nullptr;
      if (!buffer.GetData(&length_ptr)) {
        common::Log(common::Log::LogLevel::kDebug)
            << "[World] Received packet with missing size.";
        return false;
      }
      packet->data.assign(
          buffer.cbegin() + buffer.position(),
          buffer.cbegin() + buffer.position() +
              std::min(static_cast<size_t>(::be32toh(*length_ptr)),
                       buffer.remaining()));
    }
    default:
      packet->data.assign(buffer.cbegin() + buffer.position(),
                          buffer.cbegin() + buffer.position() +
                              std::min(static_cast<size_t>(kPacketSize->second),
                                       buffer.remaining()));
  }
  buffer.seek(SEEK_CUR, packet->data.size());

  common::Log(common::Log::LogLevel::kDebug)
      << "[World] Decoded packet of type "
      << std::to_string(static_cast<uint8_t>(kPacketType->second));

  return true;
}
