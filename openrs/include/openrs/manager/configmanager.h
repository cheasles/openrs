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

  enum struct FileConfig : uint16_t {
    kHitPoints = 7198,
  };

 private:
  static const std::string kDefaultConfigPath;

  std::string config_path_;
  nlohmann::json json_config_;
  std::unordered_map<uint32_t, nlohmann::json> cache_config_;

 public:
  ConfigManager(void) : config_path_(kDefaultConfigPath) {}
  ConfigManager(std::string config_path) : config_path_(config_path) {}

  /**
   * Creates the required config objects for us to manage.
   *
   * @note If no config file is found, a default one is generated.
   *
   * @return Always returns true.
   */
  bool Init() override;

  /**
   * Reads in a cache-specific config file.
   *
   * @param kVersion The Cache to associate the config with.
   * @param kPath The path of the config file to load.
   * @return Returns true if the config was found and read in, false otherwise.
   */
  bool InitCacheConfig(const uint32_t& kVersion, const std::string& kPath);

  /**
   * Generate a default config file.
   */
  void GenerateDefaultConfig();

  /**
   * Sends a variable option to the client.
   *
   * @param kPlayer The player to send the variable for.
   * @param session The client session to send the data to.
   * @param kId The ID of the variable option.
   * @param kValue The new value.
   */
  inline void SendVariable(const std::shared_ptr<openrs::game::Player>& kPlayer,
                           openrs::net::Session* session,
                           const GlobalConfig& kId,
                           const uint32_t& kValue) const {
    if (kValue > std::numeric_limits<uint8_t>::max()) {
      this->SendVariable2(kPlayer, session, kId, kValue);
    } else {
      this->SendVariable1(kPlayer, session, kId, static_cast<uint8_t>(kValue));
    }
  }

  /**
   * Sends a byte-sized global variable to the client.
   *
   * @param kPlayer The player to send the variable for.
   * @param session The client session to send the data to.
   * @param kId The ID of the variable option.
   * @param kValue The new value.
   */
  void SendVariable1(const std::shared_ptr<openrs::game::Player>& kPlayer,
                     openrs::net::Session* session, const GlobalConfig& kId,
                     const uint8_t& kValue) const;

  /**
   * Sends an int-sized variable option to the client.
   *
   * @param kPlayer The player to send the variable for.
   * @param session The client session to send the data to.
   * @param kId The ID of the variable option.
   * @param kValue The new value.
   */
  void SendVariable2(const std::shared_ptr<openrs::game::Player>& kPlayer,
                     openrs::net::Session* session, const GlobalConfig& kId,
                     const uint32_t& kValue) const;

  /**
   * Sends a global config option to the client.
   *
   * @param kPlayer The player to send the config option for.
   * @param session The client session to send the data to.
   * @param kId The ID of the global config option.
   * @param kValue The new value.
   */
  inline void SendGlobalConfig(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::net::Session* session, const GlobalConfig& kId,
      const uint32_t& kValue) const {
    if (kValue > std::numeric_limits<uint8_t>::max()) {
      this->SendGlobalConfig2(kPlayer, session, kId, kValue);
    } else {
      this->SendGlobalConfig1(kPlayer, session, kId,
                              static_cast<uint8_t>(kValue));
    }
  }

  /**
   * Sends a byte-sized global config option to the client.
   *
   * @param kPlayer The player to send the config option for.
   * @param session The client session to send the data to.
   * @param kId The ID of the global config option.
   * @param kValue The new value.
   */
  void SendGlobalConfig1(const std::shared_ptr<openrs::game::Player>& kPlayer,
                         openrs::net::Session* session, const GlobalConfig& kId,
                         const uint8_t& kValue) const;

  /**
   * Sends an int-sized global config option to the client.
   *
   * @param kPlayer The player to send the config option for.
   * @param session The client session to send the data to.
   * @param kId The ID of the global config option.
   * @param kValue The new value.
   */
  void SendGlobalConfig2(const std::shared_ptr<openrs::game::Player>& kPlayer,
                         openrs::net::Session* session, const GlobalConfig& kId,
                         const uint32_t& kValue) const;

  /**
   * Sends a file config option to the client.
   *
   * @param kPlayer The player to send the config option for.
   * @param session The client session to send the data to.
   * @param kId The ID of the file config option.
   * @param kValue The new value.
   */
  inline void SendFileConfig(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::net::Session* session, const FileConfig& kId,
      const uint8_t& kValue) const {
    if (kValue > std::numeric_limits<uint8_t>::max()) {
      this->SendFileConfig2(kPlayer, session, kId, kValue);
    } else {
      this->SendFileConfig1(kPlayer, session, kId,
                            static_cast<uint8_t>(kValue));
    }
  }

  /**
   * Sends a byte-sized file config option to the client.
   *
   * @param kPlayer The player to send the config option for.
   * @param session The client session to send the data to.
   * @param kId The ID of the file config option.
   * @param kValue The new value.
   */
  void SendFileConfig1(const std::shared_ptr<openrs::game::Player>& player,
                       openrs::net::Session* session, const FileConfig& kId,
                       const uint8_t& kValue) const;

  /**
   * Sends an int-sized file config option to the client.
   *
   * @param kPlayer The player to send the config option for.
   * @param session The client session to send the data to.
   * @param kId The ID of the file config option.
   * @param kValue The new value.
   */
  void SendFileConfig2(const std::shared_ptr<openrs::game::Player>& player,
                       openrs::net::Session* session, const FileConfig& kId,
                       const uint8_t& kValue) const;

  inline const auto& config() const { return this->json_config_; }
  auto operator[](std::string key) const { return config()[key]; }
  inline const auto& cache_config() const { return this->cache_config_; }
};

}  // namespace manager
}  // namespace openrs
