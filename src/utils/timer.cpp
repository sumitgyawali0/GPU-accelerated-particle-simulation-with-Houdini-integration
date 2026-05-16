#include "utils/timer.h"

#include <chrono>

void Timer::start()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    startTime_ =
        std::chrono::duration<double>(now).count();
}

double Timer::elapsedSeconds() const
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    const double current =
        std::chrono::duration<double>(now).count();
    return current - startTime_;
}
