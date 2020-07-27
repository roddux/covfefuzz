#include <stdint.h>
#include <stdio.h>
void check_file(uint8_t *buffer) {
//	puts("checking file");
	if (buffer[0] != 'b') return;
	if (buffer[1] != 'a') return;
	if (buffer[2] != 't') return;
	if (buffer[3] != 'm') return;
	if (buffer[4] != 'a') return;
	if (buffer[5] != 'n') return;
	*(uint16_t *)0x1337 = 0xdead;
}
