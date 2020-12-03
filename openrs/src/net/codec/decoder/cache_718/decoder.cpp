#include "OpenRS/net/codec/decoder/cache_718/decoder.h"

#include <iostream>

#include "OpenRS/net/codec/packet.h"

constexpr frozen::map<uint8_t, openrs::net::codec::PacketType, 2>
    openrs::net::codec::decoder::cache_718::Decoder::code_mapping_;
