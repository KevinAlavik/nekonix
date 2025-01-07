#include <utils/log.h>
#include <stdarg.h>
#include <utils/printf.h>
#include <nnix.h>

void _log_callback(bool debug, char *file, int line, char *level, char *scope, char *color, char *format, ...)
{
    if (debug == false || (debug && kernel_debug_enabled))
    {
        va_list args;
        va_start(args, format);
        printf("%s[%s:%d] %s @ %s: ", color, file, line, level, scope);
        vprintf(format, args);
        printf("%s\n", ANSI_COLOR_RESET);
        va_end(args);
    }
}