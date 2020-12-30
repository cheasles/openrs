#include "openrs/net/codec/decoder/global/grabdecoder.h"

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::GrabDecoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  if (buffer.remaining() < 6) {
    return false;
  }

  // Validate the priority.
  if (buffer.at(buffer.position()) > 7) {
    return false;
  }

  packet->type = PacketType::kGrabCache;
  packet->data.assign(buffer.cbegin(), buffer.cend());
  buffer.seek(SEEK_CUR, packet->data.size());

  return true;
}
