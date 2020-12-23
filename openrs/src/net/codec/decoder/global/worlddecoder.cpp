#include "openrs/net/codec/decoder/global/worlddecoder.h"

#include <iostream>

#include "openrs/net/codec/packet.h"

constexpr frozen::map<uint8_t, openrs::net::codec::PacketType, 3>
    openrs::net::codec::decoder::global::WorldDecoder::code_mapping_;

bool openrs::net::codec::decoder::global::WorldDecoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  return true;
}
