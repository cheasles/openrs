#include "openrs/net/codec/decoder/global/grabdecoder.h"

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::GrabDecoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  if (buffer.size() < 6) {
    return false;
  }

  // Validate the priority.
  if (buffer.at(0) > 7) {
    return false;
  }

  packet->type = PacketType::kGrabCache;
  packet->data.assign(buffer.cbegin(), buffer.cend());

  return true;
}
