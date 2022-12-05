#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#define EPSILON 0.001f
#define DEFUALT_ITER 200
#define MAX_ITER 1000
#define MIN_ITER 1
#define MIN_K 1
#define INVALID_CLUSTER_MSG "Invalid number of clusters! \n"
#define INVALID_ITER_MSG "Invalid maximum iteration! \n"
#define GENERAL_ERROR_MSG "An Error Has Occurred"
#define ERROR_CLEANUP_AND_EXIT(ERR_MSG)  \
	printf(ERR_MSG); \
	cleanup(); \
	exit(1)

typedef struct point
{
	double *coords;
	unsigned int dimention;
	/*
		TODO :: This is a waste of space, change it
	*/
} Point;
typedef struct clusterPoint
{
	Point point;
	unsigned int belong;
} ClusterPoint;


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

void listInit(LinkedList *);
ListNode *listInsert(LinkedList *, ListNode *);
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
int recieveFileLinkedList(LinkedList *, FILE *stream);
int parseDataPoints(LinkedList *,ClusterPoint*);
void parseDataPoint(char*,double* outData);
int extractD(char*);
/*
	Error handeling
*/
void cleanup();
/*
	User input
*/
ClusterPoint* dataPoints;
LinkedList dataPointsInput;
unsigned int n;
unsigned int k;
unsigned int iter = DEFUALT_ITER;

int main(int argc, char **argv)
{

	listInit(&dataPointsInput);

	/*
		Get all datapoints in string form
	*/
	if (recieveFileLinkedList(&dataPointsInput, stdin))
	{
		ERROR_CLEANUP_AND_EXIT(GENERAL_ERROR_MSG);
	}

	/*
		Recieve Parameter Input
	*/
	n = dataPointsInput.size;
	if(argc < 2){
		ERROR_CLEANUP_AND_EXIT(INVALID_CLUSTER_MSG);
	}
	if(argc == 3){
		iter = atoi(argv[2]);
		if(!(iter > MIN_ITER && iter < MAX_ITER))
		{
			ERROR_CLEANUP_AND_EXIT(INVALID_ITER_MSG);
		}
	}
	k = atoi(argv[1]);
	if (!(k > MIN_K && k < n))
	{
		ERROR_CLEANUP_AND_EXIT(INVALID_CLUSTER_MSG);
	}

	dataPoints = malloc(sizeof(ClusterPoint) * n);
	if(!dataPoints){
		ERROR_CLEANUP_AND_EXIT(GENERAL_ERROR_MSG);
	}
	parseDataPoints(&dataPointsInput,dataPoints);

	/*
		We finally have useable input, usable datapoint cluster array.
	*/

	return 0;
}
void listInit(LinkedList* l)
{
	l->head = NULL;
	l->size = 0;
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
	ListNode *temp;
	while (l->head != NULL)
	{
		temp = l->head;
		l->head = l->head->next;
		free(temp->key);
		free(temp);
	}
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
	size_t length;
	ssize_t nread;
	while ((nread = getline(&datapointLine, &length, stream)) != EOF)
	{
		char *temp = (char *)malloc(sizeof(char) * nread);
		if (!temp)
		{
			return 1;
		}
		stringCpy(temp, datapointLine);
		if (!listInsertKey(l, temp))
		{
			free(temp);
			return 1;
		}
		n++;
	}
	return 0;
}
int parseDataPoints(LinkedList *l,ClusterPoint*data)
{
	unsigned int dimention;
	double* temp;

	ListNode* iter = l->head;
	
	dimention = extractD(iter->key);
	
	


	while(iter!=NULL)
	{
		temp = malloc(sizeof(double)*dimention);
		if(!temp)
		{
			return 1;
		}
		parseDataPoint(iter->key,temp);
		data->point.coords = temp;
		data->point.dimention = dimention;
		data->belong = 0;

		data++;
		iter = iter->next;
	}

	return 0;
}
int extractD(char* line)
{
	int countSep = 1;
	while(*line!=EOF && *line!='\n')
	{
		if(*line == ',')
		{
			countSep++;
		}
		line++;
	}
	return countSep;
}
void parseDataPoint(char* line,double* outData)
{
	char* endI = NULL;

	while(endI == NULL || (*endI!=EOF && *endI!='\n'))
	{
		*outData = strtod(line,&endI);
		outData++;
		line+= (endI - line) + 1;
	}
}
void cleanup()
{
	size_t i;
	listDestroy(&dataPointsInput);
	for ( i = 0; i < n; i++)
	{
		free(dataPoints[i].point.coords);
	}
	free(dataPoints);
	
}
