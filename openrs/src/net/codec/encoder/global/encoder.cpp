#include "OpenRS/net/codec/encoder/global/encoder.h"

#include <iostream>

#include "OpenRS/net/codec/packet.h"

constexpr frozen::map<openrs::net::codec::PacketType, uint8_t, 2>
    openrs::net::codec::encoder::global::Encoder::code_mapping_;

bool openrs::net::codec::encoder::global::Encoder::Encode(
    const openrs::net::codec::Packet& packet,
    openrs::net::io::Buffer* buffer)
{
    if (!buffer)
    {
        return false;
    }

    const auto kPacketCode = Encoder::code_mapping_.find(packet.type);
    if (Encoder::code_mapping_.cend() == kPacketCode)
    {
        std::cerr <<
            "Invalid packet type: " <<
            std::to_string(packet.type) <<
            std::endl;
        return false;
    }

    buffer->emplace_back(kPacketCode->second);
    if (packet.data.size() != 0)
    {
        buffer->insert(
            buffer->cend(),
            packet.data.cbegin(),
            packet.data.cend());
    }

    return true;
}
