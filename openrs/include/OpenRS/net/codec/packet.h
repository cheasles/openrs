#pragma once

#include <stdint.h>

#include "OpenRS/net/io/buffer.h"

namespace openrs
{

namespace net
{

namespace codec
{

enum struct PacketType : uint8_t
{
    kUnknown,
    kHandshake,
    kLogin,
    kClientOutdated,
    kStartUp,
    kGrabCache,
};

enum struct PacketOpCode : uint8_t
{
    kNone = 255,
    kUnknown = 0,
};

struct Packet
{
    PacketType type;
    openrs::net::io::Buffer data;

private:
    template <typename Type>
    bool GetObjectFromOffset(const std::size_t& offset, Type* output)
    {
        return false;
    }
};

}  // namespace codec

}  // namespace net

}  // namespace openrs
