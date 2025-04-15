#include "cental_logger.hpp"
#include <fstream>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>

json CentralLogger::config_;
bool CentralLogger::isInitialized_ = false;

void CentralLogger::Init(const std::string& configPath) {
    if (isInitialized_) return;

    //load default config if path is not specified
    if (configPath.empty()) {
        config_ = 
        {
            {"loggers",{
                {
                
                    {"name", "default"},
                    {"level", "info"},
                    {"pattern", "[%Y-%m-%d %H:%M:%S] [%^%l%$] %v"},
                    {"sinks", {
                        {
                            {"type", "stdout"},
                            {"color", true}
                        }
                    }}
                }}
            }
            
        };
    } else {
        std::ifstream file(configPath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open logger config: " + configPath);
        }
        std::cout << "yes" << '\n'; 
        file >> config_;
    }

    //Settings up loggers from JSON
    for (const auto& loggerCfg : config_["loggers"]) {
        std::string name = loggerCfg["name"];
        std::string level = loggerCfg["level"];
        std::string pattern = loggerCfg["pattern"];

        auto logger = spdlog::get(name);
        if (!logger) {
            spdlog::sink_ptr sink;
            for (const auto& sinkCfg : loggerCfg["sinks"]) {
                std::string type = sinkCfg["type"];
                if (type == "stdout") {
                    bool useColor = sinkCfg.value("color", true);
                    sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                } else if (type == "file") {
                    std::string filename = sinkCfg["filename"];
                    sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename);
                }
                // Можно добавить другие тип (syslog, rotating file и т.д)
            }

            logger = std::make_shared<spdlog::logger>(name, sink); //?
            spdlog::register_logger(logger);//?
        }

        logger->set_level(spdlog::level::from_str(level)); //?
        logger->set_pattern(pattern);

    }

    isInitialized_ = true;
}

std::shared_ptr<spdlog::logger> CentralLogger::Get(const std::string& name) {
    if (isInitialized_) Init();
    auto logger = spdlog::get(name.empty() ? "default" : name); //?
    return logger ? logger : spdlog::default_logger(); //?
}


// {
//     "loggers": [
//         {
//             "name": "default",
//             "level": "debug",
//             "pattern": "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v",
//             "sinks": [
//                 {
//                     "type": "stdout",
//                     "color": true
//                 },
//                 {
//                     "type": "file",
//                     "filename": "logs/app.log"
//                 }
//             ]
//         },
//         {
//             "name": "network",
//             "level": "info",
//             "pattern": "[%H:%M:%S] [NET] %v",
//             "sinks": [
//                 {
//                     "type": "stdout",
//                     "color": false
//                 }
//             ]
//         }
//     ]
// }


// #include "central_logger.hpp"

// int main() {
//     // Инициализация (можно без пути — тогда дефолтные настройки)
//     CentralLogger::Init("config/logger_config.json");

//     // Получаем логгеры
//     auto defaultLogger = CentralLogger::Get();
//     auto networkLogger = CentralLogger::Get("network");

//     defaultLogger->info("This is a default log message!");
//     networkLogger->warn("Network timeout!");

//     return 0;
// }
