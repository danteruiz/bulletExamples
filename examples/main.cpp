#include "Demo.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    /* std::cout << "this is a test" << std::endl;
    auto console_sink = std::make_shared<spdlog::sinks::windebug_sink_mt>();
    //console_sink->set_level(spdlog::level::trace);
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("multisink.txt", true);
    file_sink->set_level(spdlog::level::trace);

    spdlog::logger logger("multi_sink", {console_sink, file_sink});
    logger.set_level(spdlog::level::debug);

    for (int i = 0; i < 1000; ++i) {
        logger.warn("this should appear in both console and file");
        logger.debug("this message should not appear in the console, only in the file");
    }

    //spdlog::set_default_logger(logger);


    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");*/

    DemoApplication demoApplication;
    demoApplication.exec();

    return 0;
}
