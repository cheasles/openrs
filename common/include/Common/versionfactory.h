#pragma once

#include <stdint.h>

#include <functional>

namespace openrs
{

namespace common
{

/**
 * Builds an object based on a specific version.
 */
template<typename Type, typename ... Args>
class VersionFactory
{
public:
    typedef uint32_t version;

private:
    std::map<version, std::function<Type(Args)>> registered_;

private:
    inline void RegisterBuilder(const version& version, std::function<Type(Args)>& builder)
    {
        this->registered_[version] = builder;
    }

    inline Type& Build(const version& version, Args... args) const
    {
        auto builder = this->registered_.find(version);
        if (this->registered_.cend() == builder)
        {
            throw std::runtime_error("Could not locate builder for specified version.");
        }

        return builder->second(args...);
    }
};

}  // namespace common

}  // namespace openrs

