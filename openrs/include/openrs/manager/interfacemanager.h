#pragma once

#include <openrs/common/singleton.h>
#include <openrs/game/player.h>

#include <memory>

#include "openrs/manager/manager.h"
#include "openrs/net/session.h"

namespace openrs {
namespace manager {

class InterfaceManager : public openrs::manager::Manager,
                         public openrs::common::Singleton<InterfaceManager> {
 public:
  enum struct PaneID : uint16_t {
    kFixed = 548,
    kResizable = 746,
  };
  enum struct PaneType : uint8_t {
    kNone = 0,
  };

 public:
  InterfaceManager(void) {}

  bool Init() override;

  void SendInterfaces(const std::shared_ptr<openrs::game::Player>& player,
                      openrs::net::Session* session) const;

 private:
  void SendWindowPane(const std::shared_ptr<openrs::game::Player>& player,
                      openrs::net::Session* session, const PaneID& kId,
                      const PaneType& kType) const;
};

}  // namespace manager
}  // namespace openrs
