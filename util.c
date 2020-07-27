#include "util.h"

void printMem(uint8_t sharedmem[]) {
	// Dump buffer contents
	printf("mem region:");
	for(uint32_t _=0;_<MAP_SIZE;_++) {
		if (_%32 == 0) puts(""); 
		printf("%02x ", sharedmem[_]);
	} puts("\n----");
}

void printNonBlankMem(uint8_t sharedmem[]) {
	// Dump buffer contents
	//puts("non-null bytes in mem region:");
	for(uint32_t _=0;_<MAP_SIZE;_++) {
		if(sharedmem[_] != 0x00)
			printf("%u:%02x,", _, sharedmem[_]);
	} printf("\b ");//puts("\n----");
}

uint64_t score_testcase(uint8_t sharedmem[]) {
	// Dump buffer contents
	//puts("non-null bytes in mem region:");
	uint64_t score = 0;
	for(uint64_t _=0;_<MAP_SIZE;_++) { if(sharedmem[_]!=0x00){score++;} }
	return score;
}
