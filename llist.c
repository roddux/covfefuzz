#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "llist.h"

void list_add(list *hashList, uint64_t hash, uint8_t *buffer, uint64_t buflen,
	uint64_t *listlen, uint64_t score) {
	FORWARD(hashList);
	list *X   = (list*)malloc(sizeof(list));
	X->hash   = hash;
	X->score  = score;
	X->buffer = (uint8_t*)malloc(buflen);
	memcpy(X->buffer, buffer, buflen);
	X->prev   = hashList;
	X->next   = NULL;
	if ( *listlen == 0) { // if we're adding the first element
		X->prev = NULL;
		memcpy(hashList, X, sizeof(list));
	} else {
		hashList->next = X;
	}
	(*listlen)++;
}

uint8_t in_list(list *hashList, uint64_t hash) {
	REWIND(hashList);
//	printf("\n-> START\nchecking for %lu in [ ",hash);
//	ITER(hashList, printf("%lu,",hashList->hash));
//	REWIND(hashList)
//	puts("\b ]");
	ITER(hashList, if(hashList->hash == hash) return 1; )

//	printf("\n-[ cov hash %lu not in list\n",hash);
//	REWIND(hashList);
//	printf("-[ lst ");
//	ITER(hashList, printf("%lu,",hashList->hash));
//	puts("\b]");
	return 0;
}
