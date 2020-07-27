#define REWIND(list)	while(list->prev != NULL) {\
			list=list->prev;\
			}
#define FORWARD(list)	while(list->next != NULL) {\
			list=list->next;\
			}
#define ITER(list,fun) while(1) { fun; if(list->next == NULL) break; list = list->next; }

//			printf("item->hash: %lu,item->next: %p, item->prev: %p\n", list->hash, list->next, list->prev);

typedef struct lst_mem {
	uint64_t hash;
	uint64_t score;
	uint8_t *buffer;
	struct lst_mem *prev;
	struct lst_mem *next;
} list;

extern void list_add(list *hashList, uint64_t hash, uint8_t *buffer,
			uint64_t buflen, uint64_t *listlen, uint64_t score);
extern uint8_t in_list(list *hashList, uint64_t hash);
