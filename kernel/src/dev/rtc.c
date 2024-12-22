#include <dev/rtc.h>
#include <boot/nnix.h>
#include <sys/cpu.h>
#include <dev/serial.h>

char bcd;

unsigned char read_register(unsigned char reg)
{
    __asm__ volatile("cli");
    outb(RTC_COMMAND, reg);
    __asm__ volatile("sti");
    return inb(RTC_DATA);
}

void write_register(unsigned char reg, unsigned char value)
{
    __asm__ volatile("cli");
    outb(RTC_COMMAND, reg);
    outb(RTC_DATA, value);
    __asm__ volatile("sti");
}

unsigned char bcd2bin(unsigned char in_bcd)
{
    return (bcd) ? ((in_bcd >> 4) * 10) + (in_bcd & 0x0F) : in_bcd;
}

rtc_time_point rtc_get()
{
    rtc_time_point target;

    target.seconds = bcd2bin(read_register(RTC_SECONDS));
    target.minutes = bcd2bin(read_register(RTC_MINUTES));
    target.hours = bcd2bin(read_register(RTC_HOURS));
    target.day_of_week = bcd2bin(read_register(RTC_DAY_OF_WEEK));
    target.day_of_month = bcd2bin(read_register(RTC_DAY));
    target.month = bcd2bin(read_register(RTC_MONTH));
    target.year = bcd2bin(read_register(RTC_YEAR));

    target.full[0] = target.seconds;
    target.full[1] = target.minutes;
    target.full[2] = target.hours;
    target.full[3] = target.day_of_month;
    target.full[4] = target.month;
    target.full[5] = target.year;

    return target;
}

uint64_t time_diff_seconds(rtc_time_point start, rtc_time_point end)
{
    uint64_t start_seconds =
        start.seconds + start.minutes * 60 + start.hours * 3600;
    uint64_t end_seconds = end.seconds + end.minutes * 60 + end.hours * 3600;
    return end_seconds - start_seconds;
}

uint64_t get_rtc_timestamp()
{
    uint64_t timestamp = 0;
    timestamp |= (uint64_t)(bcd2bin(read_register(RTC_YEAR)) + 2000) << 40;
    timestamp |= (uint64_t)bcd2bin(read_register(RTC_MONTH)) << 32;
    timestamp |= (uint64_t)bcd2bin(read_register(RTC_DAY)) << 24;
    timestamp |= (uint64_t)bcd2bin(read_register(RTC_HOURS)) << 16;
    timestamp |= (uint64_t)bcd2bin(read_register(RTC_MINUTES)) << 8;
    timestamp |= (uint64_t)bcd2bin(read_register(RTC_SECONDS));
    return timestamp;
}

int rtc_init()
{
    __asm__ volatile("cli");
    unsigned char status;
    status = read_register(RTC_STATUS);
    status |= 0x02;
    status |= 0x10;
    status &= ~0x20;
    status &= ~0x40;
    bcd = !(status & 0x04);
    write_register(RTC_STATUS, status);
    __asm__ volatile("sti");
    return 0;
}