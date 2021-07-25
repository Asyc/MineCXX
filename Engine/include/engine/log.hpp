#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_LOG_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_LOG_HPP_

#include <memory>

#include <spdlog/logger.h>

namespace engine::logging {
extern std::shared_ptr<spdlog::logger> g_EngineLogger;
extern std::shared_ptr<spdlog::logger> g_GameLogger;

std::shared_ptr<spdlog::logger> createLogger(const std::string_view& name);
std::shared_ptr<spdlog::logger> createLoggerDetached(const std::string_view& name);
}   // namespace engine::logging

#define MCE_LOG_DEBUG(...) ::engine::logging::g_EngineLogger->debug(__VA_ARGS__)
#define MCE_LOG_INFO(...) ::engine::logging::g_EngineLogger->info(__VA_ARGS__)
#define MCE_LOG_ERROR(...) ::engine::logging::g_EngineLogger->error(__VA_ARGS__)

#define LOG_INFO(...) ::engine::logging::g_GameLogger->info(__VA_ARGS__)
#define LOG_ERROR(...) ::engine::logging::g_GameLogger->error(__VA_ARGS__)

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_LOG_HPP_
