#pragma once

#include <endian.h>

#include <fstream>

namespace openrs
{

namespace common
{

namespace ifstream
{

inline std::size_t GetStreamSize(std::ifstream& stream)
{
    const auto kOffset = stream.tellg();
    stream.seekg(0, std::ios::beg);
    const auto kBegin = stream.tellg();
    stream.seekg(0, std::ios::end);
    const auto kSize = stream.tellg() - kBegin;
    stream.seekg(kOffset, std::ios::beg);
    return kSize;
}

template <typename Type>
inline bool ReadValue(std::ifstream& stream, Type* output)
{
    if (!output)
    {
        return false;
    }

    stream.read(reinterpret_cast<char*>(output), sizeof(Type));
    return stream.gcount() == sizeof(Type);
}

template <typename Type>
inline bool ReadValueBE(std::ifstream& stream, Type* output)
{
    if (!ReadValue(stream, output))
    {
        return false;
    }

    switch (sizeof(Type))
    {
    case sizeof(uint16_t):
        *output = ::be16toh(*output);
        break;
    case sizeof(uint32_t):
        *output = ::be32toh(*output);
        break;
    case sizeof(uint64_t):
        *output = ::be64toh(*output);
        break;
    default:
        return false;
    }

    return true;
}

template <typename Type>
inline bool ReadValueLE(std::ifstream& stream, Type* output)
{
    if (!ReadValue(stream, output))
    {
        return false;
    }

    switch (sizeof(Type))
    {
    case sizeof(uint16_t):
        *output = ::le16toh(*output);
        break;
    case sizeof(uint32_t):
        *output = ::le32toh(*output);
        break;
    case sizeof(uint64_t):
        *output = ::le64toh(*output);
        break;
    default:
        return false;
    }

    return true;
}

}  // namespace ifstream

}  // namespace common

}  // namespace openrs
