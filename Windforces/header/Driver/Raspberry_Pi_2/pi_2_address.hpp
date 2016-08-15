#ifndef _ADDRESS_
#define _ADDRESS_

extern "C"{
#include <stdio.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000)

#define BLOCK_SIZE (4 * 1024)

struct bcm2835_peripheral{
	unsigned long addr_p;
	int mem_fd;
	void *map;
	volatile unsigned int *addr;
};

struct bcm2835_peripheral gpio = {GPIO_BASE};

#endif
