#ifndef RTC_H
#define RTC_H

#include <stdint.h>

#define RTC_COMMAND 0x70
#define RTC_DATA 0x71
#define RTC_STATUS 0x0B

#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS 0x04
#define RTC_DAY_OF_WEEK 0x06
#define RTC_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09

typedef struct
{
    int miliseconds;
    int seconds;
    int minutes;
    int hours;
    int day_of_week;
    int day_of_month;
    int month;
    int year;
    int full[6];
} rtc_time_point;

int rtc_init();
rtc_time_point rtc_get();
uint64_t time_diff_seconds(rtc_time_point start, rtc_time_point end);
uint64_t get_rtc_timestamp();

#endif // RTC_H
