#include <stdio.h>
#include <stdlib.h>
#define EPSILON 0.001f
#define DEFUALT_ITER 200
#define INVALID_CLUSTER_MSG "Invalid number of clusters! \n"
#define INVALID_ITER_MSG "Invalid maximum iteration! \n"

typedef struct
{
	double key;
	struct listNode *prev;
	struct listNode *next;
} ListNode;

typedef struct
{
	ListNode *head;
} LinkedList;

void listInit(LinkedList *l);
ListNode *listInsert(LinkedList *, ListNode *);
ListNode *listInsertKey(LinkedList *, double);
ListNode *listSearch(LinkedList *, double);
void listDelete(LinkedList *, ListNode *);
void listDestroy(LinkedList *);
/*
 * Assume allocated
 */
void init(LinkedList *l)
{
	l->compare = compare;
	l->head = NULL;
}
ListNode *listInsert(LinkedList *l, ListNode *node)
{
	node->next = l->head;
	if (l->head != NULL)
	{
		l->head->prev = node;
	}
	l->head = node;
	node->prev = NULL;
	return node;
}
ListNode *listInsertKey(LinkedList *l, void *key)
{
	ListNode *node = (ListNode *)malloc(sizeof(ListNode));
	if (!node)
	{
		return NULL;
	}
	node->key = key;

	return listInsert(l, node);
}
ListNode *listSearch(LinkedList *l, void *p)
{
	ListNode *iter = l->head;
	while (iter != NULL && (l->compare(iter->key, p) != 0))
	{
		iter = iter->next;
	}
	return iter;
}
void listDelete(LinkedList *l, ListNode *p)
{
	if (p->prev != NULL)
	{
		(p->prev)->next = p->next;
	}
	else
	{
		l->head = p->next;
	}
	if (p->next != NULL)
		(p->next)->prev = p->prev;
}
void listDestroy(LinkedList *l)
{
	ListNode *iter = l->head;
	ListNode *temp;
	while (l->head != NULL)
	{
		temp = l->head;
		l->head = l->head->next;
		free(temp);
	}
	free(iter);
	free(l);
	l = NULL;
}

unsigned int k;
unsigned int iter;

int main(int argc, char **argv)
{

	/*
		READ FILE
	*/

	if (argc < 1)
	{
		printf(INVALID_CLUSTER_MSG);
	}
	else
	{
		// ADD ERROR HANDELING
		k = atoi(argv[0]);
		if (k == 0)
			iter = DEFUALT_ITER;
		if (argc == 2)
		{
			iter = atoi(argv[1]);
		}
	}
}
