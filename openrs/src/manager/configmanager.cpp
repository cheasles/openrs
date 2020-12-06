#include "OpenRS/manager/configmanager.h"

#include <iomanip>

#include <nlohmann/json.hpp>

#include "Common/log.h"

const std::string openrs::manager::ConfigManager::kDefaultConfigPath = "../data/config.json";

bool openrs::manager::ConfigManager::Init()
{
    std::ifstream input_config(this->config_path_);
    try
    {
        input_config >> this->json_config_;
    }
    catch (const nlohmann::detail::parse_error& ex)
    {
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

void openrs::manager::ConfigManager::GenerateDefaultConfig()
{
    this->json_config_.clear();
    this->json_config_["logging"]["level"] = 0;
    this->json_config_["network"]["ip_address"] = "0.0.0.0";
    this->json_config_["network"]["port"] = 43594;
    this->json_config_["grab"]["private_exponent"] = "";
    this->json_config_["grab"]["modulus"] = "";

    std::ofstream output_config(this->config_path_);
    output_config << std::setw(4) << this->json_config_ << std::endl;
}
