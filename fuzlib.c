#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

#define MAP_SIZE (1<<16)

uint8_t is_shm_setup_done = 0;
static uint8_t *fuzzing_shmem;
uint8_t prevLoc = 0;

void setup_afl_shm(void);

void __sanitizer_cov_trace_pc(void) {
	if (is_shm_setup_done == 0) {
//		puts("setting up shm");
		setup_afl_shm();
		is_shm_setup_done = 1;
	}

/*
cur_location = <COMPILE_TIME_RANDOM>;
shared_mem[cur_location ^ prev_location]++; 
prev_location = cur_location >> 1;
*/

	uintptr_t reg = (uintptr_t) __builtin_return_address(0);
	uint16_t off = (reg^prevLoc) % MAP_SIZE;

//	printf("cov[%u]++\n",off);

	fuzzing_shmem[off]++;
	prevLoc = reg>>1;
}

void setup_afl_shm(void) {
	uint32_t shmem_id = shmget(IPC_PRIVATE, MAP_SIZE, IPC_CREAT|0777);
	printf("got shm id: %u\n", shmem_id);
	fuzzing_shmem = shmat(shmem_id, NULL, 0);
	if(fuzzing_shmem == -1) {
		puts("failed to get shm. did you pass a valid ID?");
		exit(1);
	}
	printf("got shm addr: %#016x\n\n", fuzzing_shmem);
	prevLoc = random();
	memset(fuzzing_shmem,0,MAP_SIZE);
}
