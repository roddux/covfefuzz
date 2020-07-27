#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

extern void __sanitizer_cov_trace_pc(void);
extern void check_file(uint8_t *buffer); 

uint8_t buffer[1024];
sem_t *waitsem = NULL;
sem_t *waitsem2 = NULL;

void read_file(uint8_t *filename) {
//	puts("reading file");
	memset(buffer,0,1024);
	int i=0, c;
	FILE *fp = fopen(filename, "r");
	if(fp == NULL) { printf("failed to open %s\n",filename); exit(1); }
	while((c = fgetc(fp)) != EOF) buffer[i++]=c;
//	printf("buffer contents: \n%s\n", buffer);
	printf("%s\n", buffer);
	fclose(fp);
}

void wait_sem(void) {
//	puts("\nwaiting on semaphore");
	sem_wait(waitsem);
}

void create_sem(void) {
	waitsem = sem_open("/targetsem2", O_CREAT, 0777, 0);
	waitsem2 = sem_open("/targetsem3", O_CREAT, 0777, 0);
	if(waitsem == SEM_FAILED || waitsem2 == SEM_FAILED) {
		printf("failed to open semaphore: %d - %s\n",
			errno,
			strerror(errno)
		);
		exit(1);
	}
}

int main(int argc, char **argv) {
	puts("creating semaphore");
	create_sem();
	while(1) {
		wait_sem();
		read_file("/tmp/testfile");
		check_file(buffer);
		sem_post(waitsem2);
	}

	return 0;
}
