#include "util.h"
#include <fcntl.h> 
#include <errno.h> 
#include <semaphore.h> 

uint8_t testcase[1024];
sem_t *waitsem = NULL;

// Run AFL-fuzz on this client
// this client will send data to the agent
// agent will then gather coverage info and relay it back to us
// we then pass the coverage info back to afl

// do we just use TCP? what does using UDP really give us
// we are ack'ing every packet ANYWAY so it seems pointless
// tcp socket initialisation in the afl-fuzz'd client i guess

void sendTestcase(int sockfd) {
	puts("sending testcase");
	// send
}

void loadTestcase() {
	memset(&testcase, 0, sizeof(testcase));
	int i=0, c;
	while((c = fgetc(stdin)) != EOF) testcase[i++]=c;
	testcase[sizeof(testcase)-1] = '\0'; // make it printable
}

void doLog() {
	char fn[256];
	sprintf(&fn, "/tmp/fuck/log-%d.txt", random());
	FILE *const fp = fopen(fn, "wb");
	const int log_fd = fileno(fp);
	const int stdout_fd = fileno(stdout);
	const int stderr_fd = fileno(stderr);
	dup2(log_fd, stdout_fd);
	dup2(log_fd, stderr_fd);
	fclose(fp);
}

void signal_target(void) {
	waitsem = sem_open("/targetsem2", O_CREAT, 0777, 0);
	if(waitsem == SEM_FAILED) {
		printf("failed to open semaphore: %d - %s\n",
			errno,
			strerror(errno)
		);
		exit(1);
	}

	// write to file
	FILE *fp = fopen("/tmp/testfile","w");
	size_t written = fwrite(testcase, 1, sizeof(testcase), fp);
	if (written != sizeof(testcase)) {
		printf(
			"couldn't write testcase! only wrote %u of %u bytes",
			written, sizeof(testcase)
		);
		exit(1);
	}
	fclose(fp);

	sem_post(waitsem);
}

int main(int argc, char **argv) {
	srand(getpid());
	//doLog();
	puts("AFL-Client");

	// grab our AFL-provided coverage region
	char *stuff = getenv("__AFL_SHM_ID");
	if ( stuff == NULL ) {
		puts("error. pass __AFL_SHM_ID in env vars");
		exit(1);
	}
	uint32_t shmem_id = atoi(stuff);
	printf("got afl_shm_id env: '%s' (%d)\n", stuff, shmem_id);
	uint8_t *my_shmem;
	my_shmem = shmat(shmem_id, NULL, 0);
	if(my_shmem == -1) {
		puts("failed to get our shmem. did you pass a valid ID?");
		exit(1);
	}
	printf("got our shm addr: %#016x\n\n", my_shmem);

	// grab the coverage region for the target process
	// (ptrace the VM, aka argv[1])
	if ( argc != 2 ) {
		puts("wrong usage");
		exit(1);
	}
	char *stuff2 = argv[1];
	if ( stuff2 == NULL ) {
		puts("error. pass TRG_SHM_ID in env vars");
		exit(1);
	}
	shmem_id = atoi(stuff2);
	printf("got trg_shm_id env: '%s' (%d)\n", stuff2, shmem_id);
	uint8_t *their_shmem;
	their_shmem = shmat(shmem_id, NULL, 0);
	if(their_shmem == -1) {
		puts("failed to get their shmem. did you pass a valid ID?");
		exit(1);
	}
	printf("got their shm addr: %#016x\n\n", their_shmem);

#if 0
	uint32_t offset = random()%MAP_SIZE;
	uint32_t byte   = random();
	uint8_t sz      = 50;
	printf("memsetting my_shmem[%u] to be %x for %d bytes\n", offset, byte, sz);
	memset(my_shmem+offset, byte, sz);
	uint32_t _ = offset;
	printf("my_shmem[%u]: %02x%02x%02x%02x%02x%02x%02x%02x\n",
			offset,
			my_shmem[_],
			my_shmem[_+1],
			my_shmem[_+2],
			my_shmem[_+3],
			my_shmem[_+4],
			my_shmem[_+5],
			my_shmem[_+6],
			my_shmem[_+7]
	);

	// show the mem buffer
	printNonBlankMem(my_shmem);
#endif

	// read afl-provided testcase (from stdin)
	puts("reading testcase from stdin");
	loadTestcase();

	// wipe_coverage();
	puts("zeroing target's coverage");
	memset(their_shmem, 0, MAP_SIZE);

	puts("signaling target to run case");
	signal_target();
	// connect to 'target'
	// send testcase to 'target'
	// signal 'target' to run testcase

	// wait until target completes the relevant functions
	usleep(1000);

	// gather coverage from target (their shmem)
	puts("grabbing target's coverage");

#if 0
	// fake it
	uint32_t offset = random()%MAP_SIZE;
	uint32_t byte   = random();
	uint8_t sz      = 50;
	memset(their_shmem+offset, byte, sz);
#endif
//	printNonBlankMem(their_shmem);

	memcpy(my_shmem, their_shmem, MAP_SIZE);
//	printNonBlankMem(their_shmem);
//	puts("ours");
//	printNonBlankMem(their_shmem);

	// update our shmem (afl-fuzz provided) to be the new cov
	// if target dies, quit

	// send testcase to agent
	// sendTestcase();

	// wait for ACK from agent
	// recvAck(sock);

	// clear virtual machine shmem coverage region

	// signal agent to run the testcase

	// check for ACK from agent?
	// waitpid(WNOHANG) on VM

	return 0;
}
