#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"

uint32 rtc_read_low() {
  return *(volatile uint32 *)RTC_LOW;
}

uint32 rtc_read_high() {
  return *(volatile uint32 *)RTC_HIGH;
}

uint64 sys_rtc_read() {
  uint32 low, high;
  low = rtc_read_low();
  high = rtc_read_high();
  return ((uint64)high << 32) | low;
}
