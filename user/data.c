#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

#define DAYS_IN_YEAR 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
#define NS_IN_SEC 1000000000
#define SECS_IN_MIN 60
#define MINS_IN_HOUR 60
#define HOURS_IN_DAY 24
#define FIRST_YEAR 1970
char is_leap_year(int year) {
  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}
int main() {
  uint64 time_ns = rtc_read();
  uint64 time_sec = (time_ns / NS_IN_SEC);
  uint64 time_ns_rmd = (time_ns - time_sec * NS_IN_SEC);
  int time_min = time_sec / SECS_IN_MIN;
  int time_sec_rmd = time_sec % SECS_IN_MIN;
  int time_hour = time_min / MINS_IN_HOUR;
  int time_min_rmd = time_min % MINS_IN_HOUR;
  int time_day = time_hour / HOURS_IN_DAY;
  int time_hour_rmd = time_hour % HOURS_IN_DAY;
  int year = FIRST_YEAR;
  int days = time_day;
  while (days >= (is_leap_year(year) ? 366 : 365)) {
    if (is_leap_year(year)) {
      days -= 366;
    } else {
      days -= 365;
    }
    ++year;
  }
  int month = 0;
  int time_day_rmd = 0;
  int months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (is_leap_year(year)) {
    months[1]++;
  }
  for (int i = 0; i < 12; ++i) {
    if (days - months[i] < 0) {
      time_day_rmd = days;
      month = i;
      break;
    }
    days -= months[i];
  }
  printf(
      "%d year %d month %d day %d hours %d minutes %d seconds %ld nanoseconds\n",
      year, month + 1, time_day_rmd + 1, time_hour_rmd, time_min_rmd, time_sec_rmd,
      time_ns_rmd);
}
