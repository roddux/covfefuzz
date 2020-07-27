#include "util.h"
#include "llist.h"
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>
#include <dirent.h>

// TODO: sometimes the list fucks up and we dupe coverage hashes

uint8_t *testcase;
uint64_t TESTSZ;
uint64_t list_len;

sem_t *waitsem = NULL;
sem_t *waitsem2 = NULL;

extern void mutate_testcase(uint8_t *buf, uint64_t len);
extern uint64_t score_testcase(uint8_t *buf);

uint64_t hash(uint8_t *data, uint64_t len) {
	uint64_t hash = 5381;
	uint32_t c;
	uint64_t j = 0;
	for(j=0; j<len; j++, c=*data++, hash = ((hash << 5) + hash) + c);
	return hash;
}

void generate_random_testcase(uint8_t *buf, uint64_t len) {
	uint32_t i=0;
	for(i=0;i<len;buf[i++]=random());
	buf[len-1] = '\0'; // make it printable
}

// TODO: just return the pointer, don't bother memcpy() to 'testcase'
// TODO: get rid of static 'testcase' pointer, use arguments
void pick_random_testcase(list *hashList) { // assumes entries in the list
	// +50% bias toward the highest-coverage case
	if (random()%2 == 0) {
		uint8_t *mCase; uint64_t cScore = 0;
		ITER(hashList, if(hashList->score >= cScore){mCase=hashList->buffer;})
		FORWARD(hashList)
		memcpy(testcase,mCase,TESTSZ);
//		printf("-> using high-cov testcase '%s'\n", testcase);
		return;
	}

	/* +20% bias toward the newest case
	if (random()%5 == 0) {
		FORWARD(hashList)
		memcpy(testcase,hashList->buffer,TESTSZ);
		return;
	}*/

	REWIND(hashList)
	uint64_t off = random()%list_len;
	for(uint64_t j=0;j<off;j++) {
		hashList = hashList->next;
	}
	memcpy(testcase, hashList->buffer, TESTSZ);
}

void run_testcase(void) {
	// we use semaphores (hacked into target code) to signal
	// when the coverage region has been hit
	waitsem = sem_open("/targetsem2", O_CREAT, 0777, 0);
	waitsem2 = sem_open("/targetsem3", O_CREAT, 0777, 0);
	if(waitsem == SEM_FAILED || waitsem2 == SEM_FAILED) {
		printf("failed to open semaphore: %d - %s\n",
			errno,
			strerror(errno)
		);
		exit(1);
	}

	FILE *fp = fopen("/tmp/testfile","w");
	size_t written = fwrite(testcase, 1, TESTSZ, fp);
	if (written != TESTSZ) {
		printf(
			"couldn't write testcase! only wrote %lu of %lu bytes",
			written, TESTSZ
		);
		exit(1);
	}
	fclose(fp);

	sem_post(waitsem); sem_wait(waitsem2);
}

void load_testcases(list *hashList) {
	DIR *dp = opendir("/tmp/tests");
	if (dp == NULL) {
		puts("could not open directory to load testcases");
		return;
	}
	struct dirent *cur;
	uint8_t *fnam   = (uint8_t*)malloc(256);
	uint8_t *buffer = (uint8_t*)malloc(TESTSZ);
	memset(fnam,0,256);
	while ( (cur = readdir(dp)) != NULL) {
		if(cur->d_type == DT_REG) {
//			printf("got file: '%s'\n",cur->d_name);
			sprintf(fnam, "/tmp/tests/%s", cur->d_name);
			printf("opening '%s'\n", fnam);

			FILE *fp = fopen(fnam,"r");
			memset(buffer,0,TESTSZ);
			int i=0, c;
			if(fp == NULL) {
				printf("failed to open %s\n",fnam);
				exit(1);
			}
			while((c = fgetc(fp)) != EOF) buffer[i++]=c;
			fclose(fp);

			list_add(hashList,0,buffer,TESTSZ,&list_len,0);
//			list_add(myHashList,h,testcase,TESTSZ,&list_len,s);

			// READ testcase up to TESTSZ
			// add testcase to list
			if(fp==NULL) { puts("error");exit(1);}
		}
	}
	free(fnam);
}

int main(int argc, char **argv) {
	srand(getpid());
	puts("=[ b00jy fl00zy");

	TESTSZ=7;

	if ( argc != 2 || argv[1] == NULL ) {
		puts("![ error. pass target shm id as first param"); exit(1);
	}

	// attach coverage shm region for target process
	uint64_t shmem_id = atol(argv[1]);
	printf("~[ got target shm id: '%s' (%lu)\n", argv[1], shmem_id);
	int64_t *their_shmem;
	their_shmem = shmat(shmem_id, NULL, 0);
	if((int64_t)their_shmem == -1) {
		puts("![ failed to get shm. did you pass a valid ID?"); exit(1);
	}
	printf("~[ got target shm addr: %#016lx\n\n", (uint64_t)their_shmem);

	list *myHashList = (list*)malloc(sizeof(list));
	myHashList->hash   = 0;
	myHashList->buffer = NULL;
	myHashList->prev   = NULL;
	myHashList->next   = NULL;
	list_len = 0;

	//load_testcases(myHashList);
	// score the initial testcases
	//ITER(myHashList, printf("assessing testcase '%s'\n",myHashList->buffer))
	//return 0;

	// testcase = pick_random_testcase() ?
	testcase = (uint8_t*)malloc(sizeof(uint8_t)*TESTSZ);

	uint64_t cases = 0;
	// TODO: add a method to save testcases
	while(cases < 200000) { // Main loop
		if (cases%500 == 0) {
			printf("-[ round %lu, inputs %lu\n", cases, list_len);
		}

		// mutate(testcase)
		if ( cases < 2000 ) {
//		if (list_len == 0 || random()%5 == 0) {
			generate_random_testcase(testcase, TESTSZ);
		} else {
			pick_random_testcase(myHashList);
			mutate_testcase(testcase, TESTSZ);
		}

		// zero the coverage data
		memset(their_shmem, 0, MAP_SIZE);

		run_testcase();

		// check target coverage hash
		uint64_t h = hash((uint8_t*)their_shmem,MAP_SIZE);

		// save testcase if we haven't hit the same coverage already
		uint64_t s = 0;
		if (! in_list(myHashList,h)) {
			printf("--[ new cov on case %lu with: '%s'\n",
				cases,testcase);
			//printf("-[ cov hash: %lu\n-[ cov: ",h);
			//printNonBlankMem((uint8_t*)their_shmem);
			//puts("");
			s = score_testcase((uint8_t*)their_shmem);
			printf("--[ cov count/score: %lu\n", s);
			list_add(myHashList,h,testcase,TESTSZ,&list_len,s);
		}
		cases++;
	}

	return 0;
}
