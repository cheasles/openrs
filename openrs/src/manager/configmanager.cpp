#include "openrs/manager/configmanager.h"

#include <openrs/common/io/buffer.h>
#include <openrs/common/log.h>

#include <iomanip>
#include <nlohmann/json.hpp>

#include "options.h"

const std::string openrs::manager::ConfigManager::kDefaultConfigPath =
    OPENRS_CONFIG_DIR "/config.json";

bool openrs::manager::ConfigManager::Init() {
  std::ifstream input_config(this->config_path_);
  try {
    input_config >> this->json_config_;
  } catch (const nlohmann::detail::parse_error& ex) {
    common::Log(common::Log::LogLevel::kWarning)
        << "[ConfigManager] Failed to parse config file, using defaults: "
        << ex.what();
    this->GenerateDefaultConfig();
    return true;
  }

  common::Log(common::Log::LogLevel::kInfo)
      << "[ConfigManager] Loaded " << this->json_config_.size()
      << " config entries.";
  return true;
}

bool openrs::manager::ConfigManager::InitCacheConfig(const uint32_t& kVersion,
                                                     const std::string& kPath) {
  std::ifstream input_config(kPath);
  try {
    input_config >> this->cache_config_[kVersion];
  } catch (const nlohmann::detail::parse_error& ex) {
    common::Log(common::Log::LogLevel::kWarning)
        << "[ConfigManager] Failed to parse cache config file: " << ex.what();
    return false;
  }

  common::Log(common::Log::LogLevel::kInfo)
      << "[ConfigManager] Loaded " << this->cache_config_[kVersion].size()
      << " config entries for cache " << std::to_string(kVersion) << ".";
  return true;
}

void openrs::manager::ConfigManager::GenerateDefaultConfig() {
  this->json_config_.clear();
  this->json_config_["database"]["mode"] = "sqlite";
  this->json_config_["database"]["filename"] = "database.db";
  this->json_config_["logging"]["level"] = 0;
  this->json_config_["network"]["ip_address"] = "0.0.0.0";
  this->json_config_["network"]["port"] = 43594;
  this->json_config_["grab"]["keys"] = {
      175,   9857,   5907,  4981,  113430, 5558,   0,    2534, 6120,
      52303, 129045, 45253, 64569, 92184,  135106, 3940, 3909, 2447,
      150,   7416,   266,   15,    119620, 153189, 493,  436};
  this->json_config_["grab"]["private_exponent"] =
      "9577634011115533732134402962763417888862610179158224522858675069799671"
      "3454019354716577077577558156976177994479837760989691356438974879647293"
      "0641775555181875673276597933314314211532039319149338585268573964280522"
      "6692650786060316670508430284574031017830600140077767059195846665363727"
      "5131498866778592148380588481";
  this->json_config_["grab"]["modulus"] =
      "1195553312609955304946273221916548166131554766126038171030796899259954"
      "0226345789589082914809341413534242080728782003241745842876349656560597"
      "0163936696811485500553506743979521465489801746973392901885588777462023"
      "1652524839884318774110218164450587065976074532801660451229719600036298"
      "60919338852061972113876035333";
  this->json_config_["login"]["private_exponent"] =
      "905870727015513271290078916687873495093476304082150450828076282857700496"
      "642321567767556541985054129565862899813064331465033084110673586801172067"
      "320916080884184582205804790811113606564468043975607524553678626203705374"
      "610503342244481670713677434071848520578333239171703233027973563526721185"
      "95769338616589092625";
  this->json_config_["login"]["modulus"] =
      "102876637271116124732338500663639643113504464789339249490399312659674772"
      "039314875904176809267475033772367707882873773291786014475222178654932442"
      "254125731622781524413208523465520758537060408541610254619166907142593731"
      "337618490879831401461945679478046811438574041131738117063340726565226753"
      "787565780501845348613";

  std::ofstream output_config(this->config_path_);
  output_config << std::setw(4) << this->json_config_ << std::endl;
}

void openrs::manager::ConfigManager::SendGlobalConfig1(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session, const GlobalConfig& kId,
    const uint8_t& kValue) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutData<uint8_t>(-1 * kValue);
  buffer.PutShiftedPosDataBE<uint16_t>(static_cast<uint16_t>(kId));

  openrs::net::codec::Packet config_packet;
  config_packet.type = openrs::net::codec::PacketType::kConfigGlobal1;
  config_packet.data = buffer;
  session->Send(config_packet);
}

void openrs::manager::ConfigManager::SendGlobalConfig2(
    const std::shared_ptr<openrs::game::Player>& player,
    openrs::net::Session* session, const GlobalConfig& kId,
    const uint32_t& kValue) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutShiftedPosDataBE<uint16_t>(static_cast<uint16_t>(kId));
  buffer.PutData<uint32_t>(kValue);

  openrs::net::codec::Packet config_packet;
  config_packet.type = openrs::net::codec::PacketType::kConfigGlobal2;
  config_packet.data = buffer;
  session->Send(config_packet);
}
