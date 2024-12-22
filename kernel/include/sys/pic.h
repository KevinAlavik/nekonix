#ifndef PIC_H
#define PIC_H

#include <lib/types.h>

#define PIC_REMAP_OFFSET 0x20

void pic_configure(u8 offset_pic1, u8 offset_pic2, bool auto_eoi);
void pic_disable();
void pic_enable();
void pic_mask(int irq);
void pic_unmask(int irq);
void pic_send_end_of_interrupt(int irq);

u16 pic_read_irq_request_register();
u16 pic_read_in_service_register();

#endif // PIC_H