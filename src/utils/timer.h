#pragma once

class Timer
{
public:
    void start();
    double elapsedSeconds() const;

private:
    double startTime_ = 0.0;
};
