#include "logger.h"

#include <cstddef>
#include <cstdio>
#include <cstdarg>

#include "console.h"

namespace
{
LogLevel current_log_level = LogLevel::ERROR;
}

extern Console* console;

void SetLogLevel(LogLevel level)
{
    current_log_level = level;
}

int Log(LogLevel level, const char* format, ...)
{
    if (level > current_log_level)
    {
        return 0;
    }

    va_list ap;
    char buf[1024];

    va_start(ap, format);
    int result = vsprintf(buf, format, ap);
    va_end(ap);

    console->put_string(buf);
    return result;
}
