#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/shm.h>
#include <unistd.h>
#define MAP_SIZE (1<<16)

extern void printNonBlankMem(uint8_t sharedmem[]);
