#pragma once

#include <inttypes.h>

#include <nlohmann/json.hpp>

#include "Common/singleton.h"

#include "OpenRS/manager/manager.h"
#include "OpenRS/net/io/buffer.h"

namespace openrs
{

namespace manager
{

class ConfigManager
    : public openrs::manager::Manager
    , public openrs::common::Singleton<ConfigManager>
{
private:
    static const std::string kDefaultConfigPath;

    std::string config_path_;
    nlohmann::json json_config_;

public:
    ConfigManager(void)
        : config_path_(kDefaultConfigPath)
    { }
    ConfigManager(std::string config_path)
        : config_path_(config_path)
    { }

    bool Init() override;

    void GenerateDefaultConfig();

    inline const auto& config() const { return this->json_config_; }
};

}  // namespace manager

}  // namespace openrs
