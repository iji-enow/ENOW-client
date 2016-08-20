#ifndef _DRIVER_COMMON_
#define _DRIVER_COMMON_

extern "C"{
#include <stdint.h>
}

#define ERROR_TIMEOUT -1
#define ERROR_CHECKSUM -2
#define ERROR_ARGUMENT -3
#define ERROR_GPIO -4
#define SUCCESS 0

void busy_wait_milliseconds(uint32_t millis);

void sleep_milliseconds(uint32_t millis);

void set_max_priority(void);

void set_default_priority(void);

#endif

