#ifndef KERNEL_LOGGER_H
#define KERNEL_LOGGER_H

enum class LogLevel
{
    ERROR = 3,
    WARN = 4,
    INFO = 6,
    DEBUG = 7,
};

void SetLogLevel(LogLevel level);
int Log(LogLevel level, const char* format, ...);

#endif //KERNEL_LOGGER_H
