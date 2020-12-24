#include "openrs/net/codec/decoder/global/worlddecoder.h"

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::WorldDecoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  if (buffer.size() < 1) {
    return false;
  }

  const auto kPacketType = WorldDecoder::code_mapping_.find(buffer.at(0));
  if (WorldDecoder::code_mapping_.cend() == kPacketType) {
    return Decoder::Decode(buffer, packet);
  }

  packet->type = kPacketType->second;
  if (buffer.size() > 1) {
    packet->data.assign(buffer.cbegin() + sizeof(uint8_t), buffer.cend());
  }

  return true;
}
