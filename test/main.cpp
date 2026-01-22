#include <string>
#include "logging.h"

using namespace Logging;
using namespace Styling;


int main(int argc, char** argv)
{
    // Create two loggers
    Logger& logger = Logger::getLogger("testlogger");
    Logger& logger2 = Logger::getLogger("testlogger2");

    // Set the logging level of the logger and its console handler
    logger.setLevel(LogLevel::Debug);
    logger.setConsoleHandlerLevel(LogLevel::Debug);

    // Variadic log functions similar to printf and its variants
    logger.debug("This is a %s message", "debug");
    logger.info("This is an %s message", "info");
    logger.log("This is a %s message with no level prefix", "log");
    logger.warn("This is a %s message with file name and line number", "warning");
    logger.error("This is an %s message with file name and line number", "error");

    // Also provides template log functions for other types
    logger.warning(std::string{"Non-variadic function variant"});
    logger.info(42);

    logger.log(
        style(green) +
        "Styling library can be used to make text " +
        style(success|underline|italic) + "stand out"
    );

    // Logger will not log messages below the configured level
    logger2.setLevel(LogLevel::Warning);
    logger.info("This will be shown");
    logger2.info("However this will not");

    // Warning and above will write to a file in logs/
    logger.warning("This %s will be written to log file", "warning");
    logger.error("This %s will also be written to log file", "error");

    // Set a log handler to a null pointer to disable it
    logger2.setFileHandler(nullptr);
    logger2.error("This however won't be written to file");
}
