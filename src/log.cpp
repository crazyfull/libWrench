#include "log.h"

// ======= Global variable definition =======
LogLevel current_log_level = LOG_INFO;


static const char* color_codes[6] = {
    "\033[31m",
    "\033[32m",
    "\033[34m",
    "\033[33m",
    "\033[30m",
    "\033[37m"
};

static const char* level_strings[5] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};

static const char* get_color_code(enum Color color, int bold) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%s%s",
             bold ? "\033[1m" : "",
             color_codes[color]);
    return buffer;
}

static void log_message_v(LogLevel level,
                          enum Color color,
                          int bold,
                          const char* fmt,
                          va_list args)
{
    if (level < current_log_level) return;

    FILE* output = (level >= LOG_WARN) ? stderr : stdout;
    int is_tty = isatty(fileno(output));

    if (is_tty) {
        fprintf(output, "%s[%s]\033[0m ",
                get_color_code(color, bold),
                level_strings[level]);
    } else {
        fprintf(output, "[%s] ", level_strings[level]);
    }

    vfprintf(output, fmt, args);
    fprintf(output, "\n");
    fflush(output);
}


// ======= Function definitions =======

void set_log_level(LogLevel level) {
    current_log_level = level;
}

void log_level(LogLevel level, enum Color color,int bold, const char* fmt,...)
{
    va_list args;
    va_start(args, fmt);
    log_message_v(level, color, bold, fmt, args);
    va_end(args);
}
