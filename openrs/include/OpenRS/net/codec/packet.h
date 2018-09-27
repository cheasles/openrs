#pragma once

#include <stdint.h>

#include <vector>

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
};

struct Packet
{
    PacketType type;
    std::vector<uint8_t> data;

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
