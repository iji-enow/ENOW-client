extern "C"{
#include <errno.h>
#include <sched.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
}

#include "common.hpp"

void busy_wait_milliseconds(uint32_t millis){
	struct timeval deltatime;
	deltatime.tv_sec = millis / 1000;
	deltatime.tv_usec = (millis % 1000) * 1000;

	struct timeval walltime;
	gettimeofday(&walltime,\
		   	NULL);

	struct timeval endtime;
	timeradd(&walltime,\
			&deltatime,\
			&endtime);

	while(timercmp(&walltime,\
				&endtime,\
				<)){
		gettimeofday(&walltime, NULL);
	}
}

void sleep_milliseconds(uint32_t, millis){
	struct timespec sleep;
	sleep.tv_sec = millis / 1000;
	sleep.tv_nsec = (millis % 1000) * 10000000L;
	while(clock_nanosleep(CLOCK_MONOTONIC,\
				0,\
				&sleep,
				&sleep) && errno == EINTR);
}

void set_max_priority(void){
	struct sched_param sched;
	memset(&sched, 0, sizeof(sched));

	sched.sched_priority = sched_get_priority_max(SCHED_FIFO);
	sched_setschedular(0,\
			SCHED_FIFO,\
			&sched);
}

void set_default_priority(void){
	struct sched_param sched;
	memset(&sched, 0, sizeof(sched));
	sched.sched_priority = 0;
	sched_setschedular(0,\
			SCHED_OTHER,\
			&sched);
}
	
