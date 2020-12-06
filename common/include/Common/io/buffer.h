#pragma once

#include <endian.h>

#include <fstream>
#include <string>
#include <vector>

namespace openrs
{

namespace common
{

namespace io
{

template<typename Container = std::vector<uint8_t>>
class Buffer : public Container
{
private:
    typename Container::size_type position_;

public:
    Buffer()
        : position_(0)
        , Container()
    { }

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

    bool GetString(char** output)
    {
        if (!output)
        {
            return false;
        }

        if (!this->GetData<char>(output))
        {
            return false;
        }

        char* index = *output;
        while (index && *index != '\n')
        {
            if (!this->GetData<char>(&index))
            {
                return false;
            }
        }

        return true;
    }

    bool GetString(std::string* output)
    {
        if (!output)
        {
            return false;
        }

        char* c_str = nullptr;
        if (!this->GetString(&c_str))
        {
            return false;
        }

        output->assign(c_str);
        return true;
    }

    bool GetTriByte(uint32_t* output)
    {
        if (!output)
        {
            return false;
        }

        struct TriByte
        {
            uint8_t bytes[3];
        };

        TriByte* tribyte;
        if (!this->GetData<TriByte>(&tribyte))
        {
            return false;
        }

        *output = (tribyte->bytes[0] << 16) + (tribyte->bytes[1] << 8) + (tribyte->bytes[2]);
        return true;
    }

    template <typename Type>
    bool PutData(const Type data)
    {
        const auto data_ptr = reinterpret_cast<const uint8_t*>(&data);
        this->insert(
            this->cend(),
            data_ptr,
            data_ptr + sizeof(data));
        return true;
    }

    template <typename Type>
    bool PutDataBE(Type data)
    {
        switch (sizeof(Type))
        {
        case sizeof(uint16_t):
            data = ::htobe16(data);
            break;
        case sizeof(uint32_t):
            data = ::htobe32(data);
            break;
        case sizeof(uint64_t):
            data = ::htobe64(data);
            break;
        }

        return this->PutData(data);
    }

    inline void ClearOldData()
    {
        this->erase(this->cbegin(),
            this->cbegin() + this->position());
        this->position_ = 0;
    }

    inline void seek(const std::ios_base::seek_dir& dir,
        const std::vector<uint8_t>::size_type& offset)
    {
        if (dir == std::ios_base::beg)
        {
            this->position_ = offset;
        }
        else if (dir == std::ios_base::end)
        {
            this->position_ = this->size() - offset;
        }
        else if (dir == std::ios_base::cur)
        {
            this->position_ += offset;
        }
    }

    inline typename Container::size_type position() const
    {
        return this->position_;
    }
};

}  // namespace io

}  // namespace net

}  // namespace openrs
