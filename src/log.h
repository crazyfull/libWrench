#ifndef LOGING_H
#define LOGING_H
#include <cstring>
    #ifdef NDEBUG
        #define DebugPrint(...) (void)0
    #else

        #define DEBUG_FILE_NAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
        #define DebugPrint(...) do{ \
        printf("<%s::%s> ", DEBUG_FILE_NAME, __func__); \
        printf(__VA_ARGS__); \
        printf("\n");} while(0)
    #endif


    #ifdef NONEEDLOG
        #define LOG(...) (void)0
    #else
        #define LOG(...) do{ \
        printf(__VA_ARGS__); \
        printf("\n");} while(0);

    enum Color {
        Red,
        Green,
        Blue,
        Yellow,
        Black,
        White,
    };

    #define LOG_COLOR(color, ...) do{ \
        if (color == Red)   printf("\033[31m", color); \
        if (color == Green) printf("\033[32m", color); \
        if (color == Yellow)printf("\033[33m", color); \
        if (color == Blue)  printf("\033[34m", color); \
        if (color == Black)  printf("\033[30m", color); \
        if (color == White)  printf("\033[37m", color); \
        printf(__VA_ARGS__); \
    printf("\033[0m");} while(0);

    #endif

    #define ZERO (0)

    #ifdef ISINVALID
    #else
        #define ISINVALID (-1)
    #endif


#endif // LOGING_H
