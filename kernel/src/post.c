#include <boot/nnix.h>
#include <lib/stdio.h>
#include <dev/vfs.h>
#include <boot/post.h>

void sys_post(void)
{
    P_LOG("warn", "No init system available, halting system.\n");
}