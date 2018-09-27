#include "OpenRS/net/io/buffer.h"

bool openrs::net::io::Buffer::GetString(char** output)
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

bool openrs::net::io::Buffer::GetString(std::string* output)
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
