#ifndef MINECRAFTCXX_CLIENT_ENGINE_SRC_LOG_INTERNAL_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_SRC_LOG_INTERNAL_HPP_

#include <string_view>

#include <spdlog/logger.h>

namespace engine::logging {
extern spdlog::sink_ptr g_Stdout;
extern spdlog::sink_ptr g_LogFile;

void setupLogging();
void terminateLogging();
}   // namespace engine::logging

#endif //MINECRAFTCXX_CLIENT_ENGINE_SRC_LOG_INTERNAL_HPP_
