#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include <wiringPi.h>

int main(void) {
	
	key_t key = (key_t)5678;
	int shmid = 0;
	void *sharedMemoryRegion = NULL;
	char *currentAddress = NULL;

	if((shmid = shmget(key,\
					BUFSIZ,\
					0600)) == -1) {
		perror("shmget");
		exit(1);
	}

	if((sharedMemoryRegion = shmat(shmid,\
					(void *)0,\
					0)) == (void *)-1) {
		perror("shmat");
		exit(1);
	}

	wiringPiSetup();
	pinMode(2, OUTPUT);

	while(1) {
		currentAddress = (char *)sharedMemoryRegion;

		if(*currentAddress == '?') {

			printf("Message Arrived\n");
			digitalWrite(2, HIGH);
			delay(500);
			digitalWrite(2, LOW);
			delay(500);
			*currentAddress = '!';
		}
		else {
			sleep(1);
		}
	}

	return 0;
}
