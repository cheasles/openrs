#include "OpenRS/net/codec/encoder/global/encoder.h"

#include <iostream>

#include "Common/log.h"

#include "OpenRS/net/codec/packet.h"

constexpr frozen::map<openrs::net::codec::PacketType, uint8_t, 3>
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
        common::Log(common::Log::LogLevel::kWarning)
            << "Invalid packet code: " << static_cast<uint8_t>(packet.type);
        return false;
    }

    if (static_cast<uint8_t>(PacketOpCode::kNone) != kPacketCode->second)
    {
        buffer->emplace_back(kPacketCode->second);
    }
    if (packet.data.size() != 0)
    {
        buffer->insert(
            buffer->cend(),
            packet.data.cbegin(),
            packet.data.cend());
    }

    return true;
}
