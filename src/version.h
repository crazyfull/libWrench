#ifndef VERSION_H
#define VERSION_H

#define WRENCH_STR "libWrench"
#define WRENCH_VERSION_STR WRENCH_STR "1.6.13"

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_BUILD "x86_64"
#elif defined(__i386__) || defined(_M_IX86)
#define ARCH_BUILD "x86"
#elif defined(__arm__) || defined(_M_ARM)
#define ARCH_BUILD "ARM"
#elif defined(__aarch64__)
#define ARCH_BUILD "ARM64"
#else
#define ARCH_BUILD "Unknown"
#endif

#endif // VERSION_H
