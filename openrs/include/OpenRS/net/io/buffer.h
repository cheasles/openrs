#pragma once

#include <string>
#include <vector>

namespace openrs
{

namespace net
{

namespace io
{

class Buffer : public std::vector<uint8_t>
{
private:
    std::vector<uint8_t>::size_type position_;

public:
    template <typename Type>
    bool GetData(Type** output)
    {
        if (!output)
        {
            return false;
        }

        if (this->position_ + sizeof(Type) > this->size())
        {
            return false;
        }

        *output = reinterpret_cast<Type*>(&this->at(this->position_));
        this->position_ += sizeof(Type);
        return true;
    }

    bool GetString(char** output);
    bool GetString(std::string* output);

    template <typename Type>
    bool PutData(const Type& data)
    {
        this->insert(
            this->cend(),
            &data,
            &data + sizeof(data));
        return true;
    }
};

}  // namespace io

}  // namespace net

}  // namespace openrs
