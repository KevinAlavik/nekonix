#ifndef SERIAL_UTIL_H
#define SERIAL_UTIL_H

#include <dev/serial.h>

void outstr(u16 port, const char *str);
int init_serial(u16 port);
u16 serial_get_new();

#endif // SERIAL_UTIL_H