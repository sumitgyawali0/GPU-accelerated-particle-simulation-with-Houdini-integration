#include "utils/logger.h"

#include <iostream>

namespace Logger
{

namespace
{

const char* levelPrefix(Level level)
{
    switch (level)
    {
    case Level::Warn:
        return "[WARN] ";
    case Level::Error:
        return "[ERROR] ";
    case Level::Info:
    default:
        return "[INFO] ";
    }
}

} // namespace

void log(Level level, const std::string& message)
{
    std::ostream& out = (level == Level::Error) ? std::cerr : std::cout;
    out << levelPrefix(level) << message << '\n';
}

void info(const std::string& message) { log(Level::Info, message); }
void warn(const std::string& message) { log(Level::Warn, message); }
void error(const std::string& message) { log(Level::Error, message); }

} // namespace Logger
