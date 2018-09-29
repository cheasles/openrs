#pragma once

namespace openrs
{

namespace common
{

template<typename Type>
class Singleton
{
public:
    virtual ~Singleton() { }

    static Type& get()
    {
        static Type object;
        return object;
    }
};

}  // namespace common

}  // namespace openrs
