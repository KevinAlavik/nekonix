#ifndef STDIN_H
#define STDIN_H

#include <proc/manager.h>
#include <boot/nnix.h>

extern handle_t stdin_handle;

void stdin_init();

#endif // STDIN_H