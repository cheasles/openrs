#pragma once

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

}  // namespace ifstream

}  // namespace common

}  // namespace openrs
