#pragma once

#include <map>

#include "OpenRS/cache/filestore.h"
#include "OpenRS/cache/referencetable.h"

namespace openrs
{

namespace cache
{

class Container
{
public:
    Container(openrs::cache::FileStore&& store);
};

}  // namespace cache

}  // namespace openrs
