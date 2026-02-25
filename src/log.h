#ifndef LOGGING_H
#define LOGGING_H

#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <stdarg.h>

#define DEBUG_FILE_NAME ([]() -> const char* { \
    const char* slash = strrchr(__FILE__, '/'); \
    const char* backslash = strrchr(__FILE__, '\\'); \
    return slash ? slash + 1 : (backslash ? backslash + 1 : __FILE__); \
})()

#ifdef NDEBUG
#define DebugPrint(...) (void)0
#else
#define DebugPrint(...) do { \
    if (isatty(STDOUT_FILENO)) { \
        printf("\033[1;32m<%s::%s>\033[0m ", DEBUG_FILE_NAME, __func__); \
    } else { \
        printf("<%s::%s> ", DEBUG_FILE_NAME, __func__); \
    } \
    printf(__VA_ARGS__); \
    printf("\n"); \
    fflush(stdout); \
} while(0)
#endif

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_OFF
} LogLevel;

// ⬇️ فقط اعلان – تعریف منتقل شد به cpp
extern LogLevel current_log_level;

void set_log_level(LogLevel level);

enum Color {
    Red,
    Green,
    Blue,
    Yellow,
    Black,
    White
};

void log_level(LogLevel level, enum Color color, int bold, const char* fmt, ...);

#define LOG_LEVEL(level, color, bold, fmt, ...) log_level(level, color, bold, fmt, ##__VA_ARGS__)

#define LOG(fmt, ...) LOG_LEVEL(LOG_TRACE, White, 0, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG_LEVEL(LOG_DEBUG, Green, 0, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  LOG_LEVEL(LOG_INFO, Blue, 0, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG_LEVEL(LOG_WARN, Yellow, 1, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_LEVEL(LOG_ERROR, Red, 1, fmt, ##__VA_ARGS__)

#define LOG_COLOR(color, fmt, ...)  LOG_LEVEL(LOG_INFO, color, 0, fmt, ##__VA_ARGS__)
#define LOG_COLOR_BOLD(color, bold, fmt, ...)  LOG_LEVEL(LOG_INFO, color, bold, fmt, ##__VA_ARGS__)

#ifdef NDEBUG
#define DEBUG_PRINT(fmt, ...) (void)0
#else
#define DEBUG_PRINT(fmt, ...) do { \
    if (current_log_level <= LOG_DEBUG) { \
        if (isatty(STDOUT_FILENO)) { \
            printf("\033[1;32m<%s::%s>\033[0m ", DEBUG_FILE_NAME, __func__); \
        } else { \
            printf("<%s::%s> ", DEBUG_FILE_NAME, __func__); \
        } \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n"); \
        fflush(stdout); \
    } \
} while(0)
#endif

#define ZERO (0)
#ifndef ISINVALID
#define ISINVALID (-1)
#endif

#endif // LOGGING_H
