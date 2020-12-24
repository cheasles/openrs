#include "openrs/manager/worldmanager.h"

#include <openrs/common/log.h>

bool openrs::manager::WorldManager::Init() {
  this->add_world(1, openrs::game::World());

  common::Log(common::Log::LogLevel::kInfo)
      << "[WorldManager] Initialized " << this->worlds_.size()
      << " worlds.";
  return true;
}
