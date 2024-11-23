#ifndef POST_H
#define POST_H

#include <lib/types.h>
#include <lib/stdio.h>

#define P_LOG(level, fmt, ...) f_printf("[%s]: " fmt "\n", level, ##__VA_ARGS__)

#endif // POST_H