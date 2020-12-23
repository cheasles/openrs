#pragma once

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "openrs/cache/index.h"

namespace openrs {

namespace cache {

class Cache {
 private:
  std::list<std::shared_ptr<std::ifstream>> streams_;

  openrs::cache::MainIndex main_index_;
  std::map<uint32_t, openrs::cache::Index> indexes_;

 public:
  static const uint32_t kMetadataId;

 public:
  Cache(const std::string& path);

  inline const openrs::cache::Index& GetStore(const uint32_t& id) const {
    const auto& store = this->indexes_.find(id);
    if (this->indexes_.cend() == store) {
      throw std::runtime_error("Tried to access an unknown store.");
    }

    return store->second;
  }

  inline uint32_t GetTypeCount() const {
    return static_cast<uint32_t>(this->indexes_.size());
  }

  inline const auto& main_index() const { return this->main_index_; }
};

}  // namespace cache

}  // namespace openrs
