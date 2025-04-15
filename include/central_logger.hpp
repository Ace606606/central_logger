#pragma once
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CentralLogger {
public:
    //Initializing the logger from a JSON config
    static void Init(const std::string& configPath= "");
    //Get logger by name(if not specified default)
    static std::shared_ptr<spdlog::logger> Get(const std::string& name = "");

private:
    static json config_;
    static bool isInitialized_;
};
