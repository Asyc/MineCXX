#include "engine/log.hpp"

#include <array>
#include <string>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "log_internal.hpp"

namespace engine::logging {

std::shared_ptr<spdlog::logger> g_EngineLogger;
std::shared_ptr<spdlog::logger> g_GameLogger;
spdlog::sink_ptr g_Stdout;
spdlog::sink_ptr g_LogFile;

std::shared_ptr<spdlog::logger> createLogger(const std::string_view& name) {
    std::array<spdlog::sink_ptr, 2> sinks = {g_Stdout, g_LogFile};
    auto logger = std::make_shared<spdlog::async_logger>(
        std::string(name),
        sinks.begin(),
        sinks.end(),
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::block
    );

    spdlog::register_logger(logger);

#ifdef MCE_DBG
    logger->set_level(spdlog::level::debug);
#endif

    return std::move(logger);
}

void setupLogging() {
    spdlog::init_thread_pool(8192, 2);
    spdlog::flush_every(std::chrono::seconds(1));

    g_Stdout = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    g_LogFile = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/latest.log");

    g_EngineLogger = createLogger("Engine");
    g_GameLogger = createLogger("Game");
}

void terminateLogging() {
    g_Stdout.reset();
    g_LogFile.reset();

    spdlog::drop_all();
}

}   // namespace engine::logging