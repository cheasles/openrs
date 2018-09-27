#include "OpenRS/net/codec/decoder.h"

#include <iostream>

#include "OpenRS/net/codec/packet.h"

constexpr frozen::map<uint8_t, openrs::net::codec::PacketType, 2>
    openrs::net::codec::Decoder::code_mapping_;

bool openrs::net::codec::Decoder::Decode(openrs::net::io::Buffer& buffer,
    openrs::net::codec::Packet* packet)
{
    if (!packet)
    {
        return false;
    }

    if (buffer.size() < 2)
    {
        return false;
    }

    const auto kPacketType = Decoder::code_mapping_.find(buffer.at(0));
    if (Decoder::code_mapping_.cend() == kPacketType)
    {
        std::cerr <<
            "Invalid packet code: " <<
            std::to_string(buffer.at(0)) <<
            std::endl;
        return false;
    }

    const uint8_t kSize = buffer.at(1);

    const auto cbegin = buffer.cbegin() + sizeof(uint8_t) + sizeof(uint8_t);
    const auto cend = cbegin + kSize;

    if (cbegin >= buffer.cend() || cend > buffer.cend())
    {
        return false;
    }

    packet->type = kPacketType->second;
    packet->data.assign(cbegin, cend);

    return true;
}
