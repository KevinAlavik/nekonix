#ifndef LOG_H
#define LOG_H

#include <lib/types.h>

#define ANSI_COLOR_RED "\033[31m"
#define ANSI_COLOR_GREEN "\033[32m"
#define ANSI_COLOR_YELLOW "\033[33m"
#define ANSI_COLOR_BLUE "\033[34m"
#define ANSI_COLOR_MAGENTA "\033[35m"
#define ANSI_COLOR_CYAN "\033[36m"
#define ANSI_COLOR_RESET "\033[0m"

#define INFO(format, ...) _log_callback(false, __FILE__, __LINE__, "INFO", __func__, ANSI_COLOR_GREEN, format, ##__VA_ARGS__)
#define WARN(format, ...) _log_callback(false, __FILE__, __LINE__, "WARN", __func__, ANSI_COLOR_YELLOW, format, ##__VA_ARGS__)
#define ERROR(format, ...) _log_callback(false, __FILE__, __LINE__, "ERROR", __func__, ANSI_COLOR_RED, format, ##__VA_ARGS__)
#define DEBUG(format, ...) _log_callback(true, __FILE__, __LINE__, "DEBUG", __func__, ANSI_COLOR_BLUE, format, ##__VA_ARGS__)

void _log_callback(bool debug, char *file, int line, char *level, char *scope, char *color, char *format, ...);

#endif // LOG_H