#pragma once

namespace openrs
{

namespace common
{

template<typename Type>
class Singleton
{
public:
    Type& get()
    {
        static Type object;
        return object;
    }
};

}  // namespace common

}  // namespace openrs
