#pragma once

#include <map>

#include "OpenRS/FileSystem/filestore.h"
#include "OpenRS/FileSystem/referencetable.h"

namespace openrs
{

namespace filesystem
{

class Container
{
public:
    Container(openrs::filesystem::FileStore&& store);
};

}  // namespace filesystem

}  // namespace openrs
