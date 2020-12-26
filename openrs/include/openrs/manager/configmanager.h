#pragma once

#include <inttypes.h>
#include <openrs/common/io/buffer.h>
#include <openrs/common/singleton.h>
#include <openrs/game/player.h>

#include <limits>
#include <nlohmann/json.hpp>

#include "openrs/manager/manager.h"
#include "openrs/net/session.h"

namespace openrs {

namespace manager {

class ConfigManager : public openrs::manager::Manager,
                      public openrs::common::Singleton<ConfigManager> {
 public:
  enum struct GlobalConfig : uint16_t {
    k823 = 823,
  };

 private:
  static const std::string kDefaultConfigPath;

  std::string config_path_;
  nlohmann::json json_config_;

 public:
  ConfigManager(void) : config_path_(kDefaultConfigPath) {}
  ConfigManager(std::string config_path) : config_path_(config_path) {}

  bool Init() override;

  void GenerateDefaultConfig();

  inline void SendGlobalConfig(
      const std::shared_ptr<openrs::game::Player>& player,
      openrs::net::Session* session, const GlobalConfig& kId,
      const uint32_t& kValue) const {
    if (kValue > std::numeric_limits<uint8_t>::max()) {
      this->SendGlobalConfig2(player, session, kId, kValue);
    } else {
      this->SendGlobalConfig1(player, session, kId,
                              static_cast<uint8_t>(kValue));
    }
  }

  void SendGlobalConfig1(const std::shared_ptr<openrs::game::Player>& player,
                         openrs::net::Session* session, const GlobalConfig& kId,
                         const uint8_t& kValue) const;

  void SendGlobalConfig2(const std::shared_ptr<openrs::game::Player>& player,
                         openrs::net::Session* session, const GlobalConfig& kId,
                         const uint32_t& kValue) const;

  inline const auto& config() const { return this->json_config_; }
  auto operator[](std::string key) const { return config()[key]; }
};

}  // namespace manager

}  // namespace openrs
