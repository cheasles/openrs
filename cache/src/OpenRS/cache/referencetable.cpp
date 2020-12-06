#include "OpenRS/cache/referencetable.h"

int32_t ReadSmartInt(std::ifstream& stream)
{
    char flag = 0;
    stream >> flag;
    if (flag < 0)
    {
        int32_t value;
        stream >> value;
        return value & 0x7FFFFFFF;
    }
    else
    {
        int16_t value;
        stream >> value;
        return value;
    }
}

openrs::cache::ReferenceTable::ReferenceTable()
    : format_(0)
    , version_(0)
    , flags_(0)
{

}

openrs::cache::ReferenceTable::ReferenceTable(
    std::ifstream& stream)
{
    if (!stream.is_open())
    {
        throw std::runtime_error("Cache data stream is not open.");
    }

    stream >> this->format_;
    if (this->format_ < 5 || this->format_ > 7)
    {
        // Unsupported format.
        throw std::runtime_error("Cache data is not supported.");
    }

    if (this->format_ >= 6)
    {
        stream >> this->version_;
    }

    stream >> this->flags_;

    std::vector<int> ids;
    if (this->format_ >= 7)
    {
        int count = ReadSmartInt(stream);
        ids.resize(count);
    }
    else
    {
        int16_t count = 0;
        stream >> count;
        ids.resize(count);
    }

    int accumulator = 0;
    int size = -1;
    for (int i = 0; i < ids.size(); ++i)
    {
        int delta = 0;
        if (this->format_ >= 7)
        {
            delta = ReadSmartInt(stream);
        }
        else
        {
            int16_t count = 0;
            stream >> count;
            delta = count;
        }

        ids[i] = accumulator + delta;
        accumulator += delta;
        if (ids[i] > size)
            size = ids[i];
    }
    ++size;
}
