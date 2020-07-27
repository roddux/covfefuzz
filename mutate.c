#include <stdint.h>
#include <stdlib.h>

void mutate_testcase(uint8_t *buf, uint64_t len) {
	uint32_t off = random()%len;
	uint8_t flip = 0;
	switch(random()%5) {
		case 0:
			//printf("arithmetic, increment buf[%u]++\n",off);
			buf[off]++;
			break;
		case 1:
			//printf("arithmetic, decrement buf[%u]++\n",off);
			buf[off]--;
			break;
		case 2:
			//printf("bitflip, xor buf[%u]^",off);
			switch(random()%8) {
				case 0: flip=1;   break;
				case 1: flip=2;   break;
				case 2: flip=4;   break;
				case 3: flip=8;   break;
				case 4: flip=16;  break;
				case 5: flip=32;  break;
				case 6: flip=64;  break;
				case 7: flip=128; break;
			}
			buf[off] = buf[off]^flip;
			break;
		case 3:
			//printf("havok, replace byte buf[%u]\n",off);
			buf[off]=random();
			break;
		case 4:
			//printf("havok, replace 4 bytes\n");
			buf[off]=random();
			buf[random()%len]=random();
			buf[random()%len]=random();
			buf[random()%len]=random();
			break;
//		case 5:
//			//printf("interesting values-1");
//			buff[off] = interesting_values[random()%values_length];
	}
}
