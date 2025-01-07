#include <utils/log.h>
#include <stdarg.h>
#include <utils/printf.h>
#include <nnix.h>

void _log_callback(int log_level, bool debug, const char *file, int line, const char *level, const char *scope, const char *color, const char *format, ...)
{
    if (kernel_log_level >= log_level && (!debug || (debug && kernel_debug_enabled)))
    {
        va_list args;
        va_start(args, format);
        printf("%s[%-*s] (%s:%d) [%s] %s: ", color, 5, level, file, line, scope, ANSI_COLOR_RESET);
        vprintf(format, args);
        printf("\n");

        va_end(args);
    }
}
