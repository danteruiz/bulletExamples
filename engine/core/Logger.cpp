#include "Logger.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/wincolor_sink.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/wincolor_sink.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <memory>
namespace logger
{
    void initializeSpdLog()
    {

#ifdef _WIN32
        auto consoleSink = std::make_shared<spdlog::sinks::windebug_sink_mt>();
#else
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#endif

        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("general.txt", true);

        consoleSink->set_level(spdlog::level::trace);
        fileSink->set_level(spdlog::level::trace);

        auto logger = std::make_shared<spdlog::logger>("general");
        logger->set_level(spdlog::level::debug);
        auto& sinks = logger->sinks();
        sinks.push_back(consoleSink);
        sinks.push_back(fileSink);
        spdlog::set_default_logger(logger);
        spdlog::flush_every(std::chrono::seconds(1));
    }
}
