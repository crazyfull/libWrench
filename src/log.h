#ifndef LOGING_H
#define LOGING_H
#include "version"
#include <cstring>
#include <cstdio>
#include <unistd.h>

#define DEBUG_FILE_NAME ([]() -> const char* { \
    const char* slash = strrchr(__FILE__, '/'); \
    const char* backslash = strrchr(__FILE__, '\\'); \
    return slash ? slash + 1 : (backslash ? backslash + 1 : __FILE__); \
})()

#ifdef NDEBUG
    #define DebugPrint(...) (void)0
#else
    #define DebugPrint(...) do { \
        printf("\033[1;32m<%s::%s>\033[0m ", DEBUG_FILE_NAME, __func__); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        fflush(stdout); \
    } while(0)
#endif

#ifdef NONEEDLOG
    #define LOG(...) (void)0
#else
    #define LOG(...) do { \
        printf(__VA_ARGS__); \
        printf("\n"); \
        fflush(stdout); \
    } while(0)
#endif

#ifndef LOG_COLOR_ENUM_DEFINED
#define LOG_COLOR_ENUM_DEFINED
    enum Color {
        Red,
        Green,
        Blue,
        Yellow,
        Black,
        White,
    };
#endif

#define LOG_COLOR(color, ...) do { \
    if (!isatty(STDOUT_FILENO)) { \
            /* NOT a terminal → behave exactly like LOG */ \
            printf(__VA_ARGS__); \
            printf("\n"); \
    } else { \
            const char* color_code = ""; \
            switch (color) { \
            case Red:    color_code = "\033[31m"; break; \
            case Green:  color_code = "\033[32m"; break; \
            case Yellow: color_code = "\033[33m"; break; \
            case Blue:   color_code = "\033[34m"; break; \
            case Black:  color_code = "\033[30m"; break; \
            case White:  color_code = "\033[37m"; break; \
        } \
            printf("%s", color_code); \
            printf(__VA_ARGS__); \
            printf("\033[0m\n"); \
    } \
        fflush(stdout); \
    } while(0)

#define ZERO (0)

#ifndef ISINVALID
#define ISINVALID (-1)
#endif

#endif // LOGING_H
