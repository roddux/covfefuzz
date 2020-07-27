CFLAGS=-lpthread -g -O0 -funroll-loops

all: clean fuzlib target client
client:
fuzlib:
	$(CC) $(CFLAGS) -c fuzlib.c -o fuzlib.o
	#$(CC) -shared -o fuzlib.so fuzlib.o
target:
	#$(CC) $(CFLAGS) -fsanitize-coverage=trace-cmp,trace-pc target.c -o target
	$(CC) $(CFLAGS) -fsanitize-coverage=trace-pc target-lib.c -c -o target-lib.o
	$(CC) $(CFLAGS) target.c target-lib.o fuzlib.o -o target
fuzzy:
	$(CC) $(CFLAGS) -Wall -Wextra -pedantic fuzzy.c util.c mutate.c llist.c
clean:
	rm -f ./client ./target ./init ./a.out ./fuzlib.o ./fuzlib.so
	rm -f ./core.*
.PHONY: clean
