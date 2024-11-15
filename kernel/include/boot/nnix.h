#ifndef NNIX_H
#define NNIX_H

#include <lib/stdio.h>

#ifndef VERSION_MAJOR
#define VERSION_MAJOR "0"
#endif // VERSION_MAJOR

#ifndef VERSION_MINOR
#define VERSION_MINOR "0"
#endif // VERSION_MINOR

#ifndef VERSION_PATCH
#define VERSION_PATCH "0"
#endif // VERSION_PATCH

#ifndef VERSION_NOTE
#define VERSION_NOTE "-unkown"
#endif // VERSION_NOTE

#ifndef _GRAPHICAL_LOG
#define _GRAPHICAL_LOG 0
#endif // _GRAPHICAL_LOG

#ifndef _MIRROR_LOG
#define _MIRROR_LOG 0
#endif // MIRROR_LOG

#define _LOG(scope, level, fmt, ...) \
    printf("nnix@%s:\t[%s]\t -> " fmt "\n", scope, level, ##__VA_ARGS__);

#define INFO(scope, fmt, ...) _LOG(scope, "INFO", fmt, ##__VA_ARGS__)
#ifdef _DEBUG
#define DEBUG(scope, fmt, ...) _LOG(scope, "DEBUG", fmt, ##__VA_ARGS__)
#else
#define DEBUG(scope, fmt, ...) (void)0
#endif // _DEBUG
#define NOTE(scope, fmt, ...) _LOG(scope, "NOTE", fmt, ##__VA_ARGS__)
#define WARN(scope, fmt, ...) _LOG(scope, "WARN", fmt, ##__VA_ARGS__)
#define ERROR(scope, fmt, ...) _LOG(scope, "ERROR", fmt, ##__VA_ARGS__)

#endif // NNIX_H