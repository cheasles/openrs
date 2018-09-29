#pragma once

#include <stdint.h>

#include "OpenRS/net/io/buffer.h"

namespace openrs
{

namespace net
{

namespace codec
{

enum PacketType
{
    kUnknown,
    kHandshake,
    kLogin,
    kClientOutdated,
    kGrabKeys,
    kGrabCache,
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
