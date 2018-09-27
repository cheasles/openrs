#pragma once

#include <stdint.h>

#include <map>

#include <frozen/map.h>

#include "OpenRS/net/codec/packet.h"
#include "OpenRS/net/io/buffer.h"

namespace openrs
{

namespace net
{

namespace codec
{

class Decoder
{
private:
    static constexpr frozen::map<uint8_t, PacketType, 2> code_mapping_
    {
        {15, PacketType::kHandshake},
        {14, PacketType::kLogin},
    };

public:
    virtual bool Decode(openrs::net::io::Buffer& buffer,
        openrs::net::codec::Packet* packet);
};

}  // namespace codec

}  // namespace net

}  // namespace openrs
