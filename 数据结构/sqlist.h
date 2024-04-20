typedef int data_t;
#define N 128
typedef struct
{
	data_t data;
	int last;
}node, *pnode

node = sqlist_create(void);
int list_clear(pnode L);
int list_free(pnode L);
int list_empty(pnode L);
int list_length(pnode L);
int list_locate(pnode L, data_t value);
int list_insert(pnode L, data_t value);
int list_delete(pnode L, int pos);
int list_merge(pnode L1, pnode L2);
int list_purge(pnode L);
