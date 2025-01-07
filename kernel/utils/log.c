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

        printf("%s[%s] (%s:%d) [%s] %s: ", color, level, file, line, scope, ANSI_COLOR_RESET);
        vprintf(format, args);
        printf("\n");

        va_end(args);
    }
}
