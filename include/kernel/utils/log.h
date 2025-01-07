#ifndef LOG_H
#define LOG_H

#include <lib/types.h>

// Softer color definitions for better readability
#define ANSI_COLOR_RED "\033[38;5;9m"
#define ANSI_COLOR_GREEN "\033[38;5;10m"
#define ANSI_COLOR_YELLOW "\033[38;5;11m"
#define ANSI_COLOR_BLUE "\033[38;5;12m"
#define ANSI_COLOR_MAGENTA "\033[38;5;13m"
#define ANSI_COLOR_CYAN "\033[38;5;14m"
#define ANSI_COLOR_RESET "\033[0m"

#ifndef KLOG_MODULE
#define KLOG_MODULE "kernel"
#endif // KLOG_MODULE

// Critical logs, always present.
#define NOTE(format, ...) _log_callback(0, false, __FILE__, __LINE__, "NOTE", KLOG_MODULE, ANSI_COLOR_MAGENTA, format, ##__VA_ARGS__)
#define PANIC(format, ...) _log_callback(0, false, __FILE__, __LINE__, "PANIC", KLOG_MODULE, ANSI_COLOR_RED, format, ##__VA_ARGS__)

// Error logs, only present if loglevels > 0.
#define ERROR(format, ...) _log_callback(1, false, __FILE__, __LINE__, "ERROR", KLOG_MODULE, ANSI_COLOR_RED, format, ##__VA_ARGS__)
#define WARN(format, ...) _log_callback(1, false, __FILE__, __LINE__, "WARN", KLOG_MODULE, ANSI_COLOR_YELLOW, format, ##__VA_ARGS__)

// Info logs, only present if loglevels > 1.
#define INFO(format, ...) _log_callback(2, false, __FILE__, __LINE__, "INFO", KLOG_MODULE, ANSI_COLOR_GREEN, format, ##__VA_ARGS__)

// Debug logs, only present if loglevels > 2.
#define DEBUG(format, ...) _log_callback(3, true, __FILE__, __LINE__, "DEBUG", KLOG_MODULE, ANSI_COLOR_BLUE, format, ##__VA_ARGS__)

// Trace logs, only present if loglevels > 3.
#define TRACE(format, ...) _log_callback(4, true, __FILE__, __LINE__, "TRACE", KLOG_MODULE, ANSI_COLOR_CYAN, format, ##__VA_ARGS__)

void _log_callback(int log_level, bool debug, const char *file, int line, const char *level, const char *scope, const char *color, const char *format, ...);

#endif // LOG_H
