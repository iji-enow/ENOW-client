#ifndef __MMIO_HPP__
#define __MMIO_HPP__

#include "pi_2_address.hpp"

extern "C"{
#include <stdint.h>
}

#define MMIO_SUCCESS 0
#define MMIO_ERROR_DEVMEM -1
#define MMIO_ERROR_MMAP -2
#define MMIO_ERROR_OFFSET -3

/*
 * extern volatile uint32_t *pi_2_mmio_gpio;
 */

/*
 * struct bcm2835_peripheral{
 * 	unsigned long addr_p;
 * 	int mem_fd;
 * 	void *map;
 * 	volatile unsigned int *addr;
 * };
 */

extern struct bcm2835_peripheral gpio;

static inline void pi_2_mmio_set_input(const int gpio_number){
	*(gpio.addr + ((gpio_number) / 10)) &= ~(7 << ( ( (gpio_number) % 10) * 3) );
}

static inline void pi_2_mmio_set_output(const int gpio_number){
	pi_2_mmio_set_input(gpio_number);
	*(gpio.addr + ( (gpio_number) / 10) ) |= (1 << ( ( (gpio_number) % 10)*3) );
}

static inline void pi_2_mmio_set_gpio_alt(const int gpio_number, const int alternative){
	*(gpio.addr + ( ( (gpio_number) / 10) ) ) |= ( ( (alternative) <= 3 ? (alternative) + 4 : (alternative) == 4 ? 3 : 2) << ( ( (gpio_number) % 10) * 3) );
}

static inline void pi_2_mmio_set_high(const int gpio_number){
	*(gpio.addr + 7) = 1 << gpio_number;
}

static inline void pi_2_mmio_set_low(const int gpio_number){
	*(gpio.addr + 10) = 1 << gpio_number;
}

static inline uint32_t pi_2_mmio_input(const int gpio_number){
	return *(gpio.addr + 13) & (1 << gpio_number);
}

#endif
