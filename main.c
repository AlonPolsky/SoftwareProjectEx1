#include <stdio.h>
#include <stdlib.h>
#define EPSILON 0.001f
#define DEFUALT_ITER 200
#define INVALID_CLUSTER_MSG "Invalid number of clusters! \n"
#define INVALID_ITER_MSG "Invalid maximum iteration! \n"
#define GENERAL_ERROR "An Error Has Occurred"

/*
	Linked List
*/
typedef struct listNode
{
	char *key;
	struct listNode *prev;
	struct listNode *next;
} ListNode;

typedef struct
{
	ListNode *head;
} LinkedList;

int listInit(LinkedList *l);
ListNode *listInsert(LinkedList *l, ListNode *);
ListNode *listInsertKey(LinkedList *, char *);
void listDelete(LinkedList *, ListNode *);
void listDestroy(LinkedList *);

/*
	String Actions
	Assumes strings of the same length
*/
void stringCpy(char *copyInto, char *copyFrom);

/*
	User input
*/
LinkedList *dataPoints;
unsigned int n;
unsigned int k;
unsigned int iter;

/*
	File Read Help
*/
char *datapointLine;
unsigned int length;
size_t nread;

int main(int argc, char **argv)
{

	if (!listInit(dataPoints))
	{
		printf(GENERAL_ERROR);
	}
	n = 0;
	while ((nread = getline(&datapointLine, &length, stdin)) != EOF)
	{
		char *temp = (char *)malloc(sizeof(char) * nread);
		stringCpy(temp, datapointLine);
		listInsertKey(dataPoints, temp);
		n++;
	}

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
int listInit(LinkedList *l)
{
	l = (LinkedList *)malloc(sizeof(LinkedList));
	if (!l)
		return 1;
	l->head = NULL;
	return 0;
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
ListNode *listInsertKey(LinkedList *l, char *key)
{
	ListNode *node = (ListNode *)malloc(sizeof(ListNode));
	if (!node)
	{
		return NULL;
	}
	node->key = key;

	return listInsert(l, node);
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
		free(temp->key);
		free(temp);
	}
	free(iter);
	free(l);
	l = NULL;
}
void stringCpy(char *copyInto, char *copyFrom)
{
	while (*copyFrom != '\0')
	{
		*copyInto = *copyFrom;
		copyInto++;
		copyFrom++;
	}
}