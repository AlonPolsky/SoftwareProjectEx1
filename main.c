#include <stdio.h>
#include <stdlib.h>
#define EPSILON 0.001f
#define DEFUALT_ITER 200
#define INVALID_CLUSTER_MSG "Invalid number of clusters! \n"
#define INVALID_ITER_MSG "Invalid maximum iteration! \n"
#define GENERAL_ERROR_MSG "An Error Has Occurred"
#define ERROR_CLEANUP_AND_EXIT(ERR_MSG)  \
	printf(ERR_MSG); \
	cleanup(); \ 
	exit(1);

typedef struct point
{
	double *coords;
	unsigned int dimention;
} Point;
typedef struct clusterPoint
{
	Point point;
	unsigned int belong;
} ClusterPoint;

ClusterPoint dataPoints[];
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
	unsigned int size;
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
	Input handeling
*/
int recieveFileLinkedList(LinkedList *l, FILE *stream);
/*
	User input
*/
LinkedList *dataPointsInput;
unsigned int n;
unsigned int k;
unsigned int iter;

int main(int argc, char **argv)
{

	if (!listInit(dataPointsInput))
	{
		ERROR_CLEANUP_AND_EXIT(GENERAL_ERROR);
	}
	/*
		Get all datapoints in string form
	*/
	if (!recieveFileLinkedList(dataPointsInput, stdin))
	{
		ERROR_CLEANUP_AND_EXIT(GENERAL_ERROR);
	}
	n = dataPointsInput->size;

	if (argc < 1)
	{
		ERROR_CLEANUP_AND_EXIT(INVALID_CLUSTER_MSG);
	}
	else
	{
		// ADD ERROR HANDELING
		k = atoi(argv[0]);
		if (!(k > 1 && k < n))
		{
			ERROR_CLEANUP_AND_EXIT(INVALID_CLUSTER_MSG);
		}
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
	l->size = 0;
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
	l->size++;
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
	l->size--;
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
int recieveFileLinkedList(LinkedList *l, FILE *stream)
{
	char *datapointLine;
	unsigned int length;
	size_t nread;

	// TODO:: CHECK IF NEEDED
	length = 0;
	datapointLine = NULL;

	while ((nread = getline(&datapointLine, &length, stream)) != EOF)
	{
		char *temp = (char *)malloc(sizeof(char) * nread);
		if (!temp)
		{
			return 1;
		}
		stringCpy(temp, datapointLine);
		if (!listInsertKey(dataPointsInput, temp))
		{
			return 1;
		}
		n++;
	}
	return 0;
}