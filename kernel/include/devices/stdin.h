#ifndef STDIN_H
#define STDIN_H

#include <proc/manager.h>
#include <boot/nnix.h>

typedef struct
{
    const char *sym;
    u8 scancode;
    bool released;
} key_t;

extern handle_t stdin_handle;

void stdin_init();

#endif // STDIN_H