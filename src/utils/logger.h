#pragma once

#include <string>

namespace Logger
{

enum class Level
{
    Info,
    Warn,
    Error
};

void log(Level level, const std::string& message);
void info(const std::string& message);
void warn(const std::string& message);
void error(const std::string& message);

} // namespace Logger
