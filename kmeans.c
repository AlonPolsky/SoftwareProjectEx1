#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 0.001
#define DEFUALT_ITER 200
#define MAX_ITER 1000
#define MIN_ITER 1
#define MIN_K 1
#define INVALID_CLUSTER_MSG "Invalid number of clusters! \n"
#define INVALID_ITER_MSG "Invalid maximum iteration! \n"
#define GENERAL_ERROR_MSG "An Error Has Occurred"
#define ERROR_CLEANUP_AND_EXIT(ERR_MSG) \
	printf(ERR_MSG);                    \
	cleanup();                          \
	return 1

typedef struct point
{
	double *coords;
	unsigned int dimention;
	/*
		TODO :: This is a waste of space, change it
	*/
} Point;
typedef Point Centroid;
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
void listReverse(LinkedList *);
/*
	String Actions
	Assumes strings of the same length
*/
void stringCpy(char *copyInto, char *copyFrom);

/*
	IO handeling
*/
int recieveFileLinkedList(LinkedList *, FILE *stream);
int parseDataPoints(LinkedList *, ClusterPoint *);
void parseDataPoint(char *, double *outData);
int extractD(char *);
void printResults(const Centroid *finalCentroids, const unsigned int k);
/*
	Actual logic
*/
void initDataPointsWithStartingCluster(ClusterPoint *, Centroid *, unsigned int);
int initPointArr(Point *, unsigned int, unsigned int);
void updateDataPointCluster(ClusterPoint *data, const unsigned int dataSize, Centroid *centeroids, const unsigned int centeroidsSize);
int mmDistanceIndex(const Point *point, const Point *otherPoints, const unsigned int otherPointsSize, const int operator);
int minDistanceIndex(const Point *point, const Point *otherPoints, const unsigned int otherPointsSize);
int maxDistanceIndex(const Point *point, const Point *otherPoints, const unsigned int otherPointsSize);
double euclideanDistance(const Point *a, const Point *b);
void sumIntoPoint(Point *a, const Point *b);
void multipyPointByScalar(const Point *p, double scalar);
void copyIntoPoint(Point *copyInto, const Point *copyFrom);
int getUpdatedCentroids(const ClusterPoint *clusterPoints, const unsigned int n, const unsigned int k, Centroid *updatedCentroids);
/*
	Error handeling and cleanup
*/
void cleanup();
void centroidsDestroy(Centroid *, unsigned int k);
void pointDestroy(Point *, unsigned int k);
void listDestroy(LinkedList *);
void coordsDestroy(Point *pArr, unsigned int k);

/*
	User input
*/
ClusterPoint *dataPoints = NULL;
Centroid *centroids = NULL;
Centroid *updatedCentroids = NULL;

LinkedList dataPointsInput;
unsigned int n = 0;
unsigned int k = 0;
unsigned int iter = DEFUALT_ITER;

int main(int argc, char **argv)
{
	unsigned int itersCompleted = 0;
	size_t i = 0;
	unsigned int converged = 0;
	unsigned int status;
	double maxDelta;
	double tempDelta;

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
	if (argc < 2 || argc > 3)
	{
		ERROR_CLEANUP_AND_EXIT(INVALID_CLUSTER_MSG);
	}
	if (argc == 3)
	{
		iter = atoi(argv[2]);
		if (!(iter > MIN_ITER && iter < MAX_ITER))
		{
			ERROR_CLEANUP_AND_EXIT(INVALID_ITER_MSG);
		}
	}
	k = atoi(argv[1]);
	if (!(k > MIN_K && k < n))
	{
		ERROR_CLEANUP_AND_EXIT(INVALID_CLUSTER_MSG);
	}

	dataPoints = (ClusterPoint *)malloc(sizeof(ClusterPoint) * n);
	centroids = (Centroid *)malloc(sizeof(Centroid) * k);
	updatedCentroids = (Centroid *)malloc(sizeof(Centroid) * k);

	if (dataPoints == NULL || centroids == NULL || updatedCentroids == NULL)
	{
		ERROR_CLEANUP_AND_EXIT(GENERAL_ERROR_MSG);
	}
	listReverse(&dataPointsInput);
	parseDataPoints(&dataPointsInput, dataPoints);

	/*
		We finally have useable input, usable datapoint cluster array.
		init It according to step 1 in the algorithm
	*/
	initDataPointsWithStartingCluster(dataPoints, centroids, k);
	status = initPointArr(updatedCentroids, dataPoints->point.dimention, k);
	if (status == 1)
	{
		return 1;
	}
	while (!converged && itersCompleted < iter)
	{

		/*
			Assign every xi to the closest cluster
		*/
		updateDataPointCluster(dataPoints, n, centroids, k);

		/*
			updatedCentroids are updated here
		*/
		if (getUpdatedCentroids(dataPoints, n, k, updatedCentroids) == 1)
		{
			ERROR_CLEANUP_AND_EXIT(GENERAL_ERROR_MSG);
		}
		maxDelta = 0;
		for (i = 0; i < k; i++)
		{
			tempDelta = euclideanDistance(&updatedCentroids[i], &centroids[i]);
			if (tempDelta > maxDelta)
			{
				maxDelta = tempDelta;
			}
		}
		if (maxDelta < EPSILON)
		{
			converged = 1;
		}
		for (i = 0; i < k; i++)
		{
			copyIntoPoint(&centroids[i], &updatedCentroids[i]);
		}

		itersCompleted++;
	}

	printResults(centroids, k);
	/*
		Finished, Cleanup
	*/
	cleanup();
	return 0;
}
void listInit(LinkedList *l)
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
	if (node == NULL)
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
void listReverse(LinkedList *l)
{
	ListNode *prev = NULL;
	ListNode *curr = l->head;
	ListNode *next;
	while (curr != NULL)
	{
		next = curr->next;
		curr->next = prev;
		prev = curr;
		curr = next;
	}
	l->head = prev;
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
	char *datapointLine = NULL;
	size_t length = 0;
	ssize_t nread;
	while ((nread = getline(&datapointLine, &length, stream)) != EOF)
	{
		char *temp = (char *)malloc(sizeof(char) * nread);
		if (temp == NULL)
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
	free(datapointLine);
	return 0;
}
int parseDataPoints(LinkedList *l, ClusterPoint *data)
{
	unsigned int dimention;
	double *temp;

	ListNode *iter = l->head;

	dimention = extractD(iter->key);

	while (iter != NULL)
	{
		temp = malloc(sizeof(double) * dimention);
		if (temp == NULL)
		{
			return 1;
		}
		parseDataPoint(iter->key, temp);
		data->point.coords = temp;
		data->point.dimention = dimention;
		data->belong = 0;

		data++;
		iter = iter->next;
	}

	return 0;
}
int extractD(char *line)
{
	int countSep = 1;
	while (*line != EOF && *line != '\n')
	{
		if (*line == ',')
		{
			countSep++;
		}
		line++;
	}
	return countSep;
}
void initDataPointsWithStartingCluster(ClusterPoint *clusterPoints, Centroid *clusterCentroids, unsigned int clusterCount)
{
	size_t i;
	initPointArr(clusterCentroids, clusterPoints[0].point.dimention, clusterCount);
	for (i = 0; i < clusterCount; i++)
	{

		copyIntoPoint(&clusterCentroids[i], &clusterPoints[i].point);
		clusterPoints[i].belong = i + 1;
	}
}
void parseDataPoint(char *line, double *outData)
{
	char *endI = NULL;

	while (endI == NULL || (*endI != EOF && *endI != '\n'))
	{
		*outData = strtod(line, &endI);
		outData++;
		line += (endI - line) + 1;
	}
}
void updateDataPointCluster(ClusterPoint *data, const unsigned int dataSize, Centroid *centeroids, const unsigned int centeroidsSize)
{
	size_t i;
	int minCluster;
	for (i = 0; i < dataSize; i++)
	{
		minCluster = minDistanceIndex(&data[i].point, centeroids, centeroidsSize) + 1;
		data[i].belong = minCluster;
	}
}
int minDistanceIndex(const Point *point, const Point *otherPoints, const unsigned int otherPointsSize)
{
	return mmDistanceIndex(point, otherPoints, otherPointsSize, 0);
}
int maxDistanceIndex(const Point *point, const Point *otherPoints, const unsigned int otherPointsSize)
{
	return mmDistanceIndex(point, otherPoints, otherPointsSize, 1);
}
int mmDistanceIndex(const Point *point, const Point *otherPoints, const unsigned int otherPointsSize, const int operator)
{
	size_t i;
	unsigned int minIndex = 0;
	double minDistance = euclideanDistance(point, &otherPoints[0]);
	double tempDistance;
	for (i = 1; i < otherPointsSize; i++)
	{
		tempDistance = euclideanDistance(point, &centroids[i]);
		if (operator== 0 ? (tempDistance < minDistance) : (tempDistance > minDistance))
		{
			minIndex = i;
			minDistance = tempDistance;
		}
	}
	return minIndex;
}
double euclideanDistance(const Point *a, const Point *b)
{
	double sum = 0;
	size_t i = 0;
	for (; i < a->dimention; i++)
	{
		sum += pow((a->coords[i] - b->coords[i]), 2);
	}
	return sqrt(sum);
}
int getUpdatedCentroids(const ClusterPoint *clusterPoints, const unsigned int n, const unsigned int k, Centroid *updatedCentroids)
{

	size_t i;
	unsigned int dim = clusterPoints[0].point.dimention;
	int *clusterSizeArr = (int *)calloc(k, sizeof(int));
	Point *clusterSumArr = (Point *)calloc(k, sizeof(Point));
	initPointArr(clusterSumArr, dim, k);

	if (updatedCentroids == NULL || clusterSizeArr == NULL || clusterSumArr == NULL)
	{
		free(clusterSizeArr);
		free(clusterSumArr);
		return 1;
	}

	for (i = 0; i < n; i++)
	{
		clusterSizeArr[clusterPoints[i].belong - 1] += 1;
		sumIntoPoint(&clusterSumArr[clusterPoints[i].belong - 1], &clusterPoints[i].point);
	}

	for (i = 0; i < k; i++)
	{
		if (clusterSizeArr[i] == 0)
		{
			free(clusterSizeArr);
			free(clusterSumArr);
			return 1;
		}
		multipyPointByScalar(&clusterSumArr[i], 1 / ((double)clusterSizeArr[i]));
		copyIntoPoint(&updatedCentroids[i], &clusterSumArr[i]);
	}
	pointDestroy(clusterSumArr, k);
	free(clusterSizeArr);
	return 0;
}
void sumIntoPoint(Point *sumInto, const Point *sumFrom)
{
	size_t i;
	for (i = 0; i < sumInto->dimention; i++)
	{
		sumInto->coords[i] += sumFrom->coords[i];
	}
}
void multipyPointByScalar(const Point *p, double scalar)
{
	size_t i;
	for (i = 0; i < p->dimention; i++)
	{
		p->coords[i] *= scalar;
	}
}
void copyIntoPoint(Point *copyInto, const Point *copyFrom)
{
	size_t i;
	for (i = 0; i < copyInto->dimention; i++)
	{
		copyInto->coords[i] = copyFrom->coords[i];
	}
}

void printResults(const Centroid *finalCentroids, const unsigned int k)
{
	size_t i, j;
	for (i = 0; i < k; i++)
	{
		for (j = 0; j < finalCentroids->dimention; j++)
		{
			printf("%.4f", finalCentroids[i].coords[j]);
			if (j != finalCentroids->dimention - 1)
			{
				printf(",");
			}
		}
		printf("\n");
	}
}
int initPointArr(Point *arr, unsigned int dim, unsigned int k)
{
	size_t i;
	for (i = 0; i < k; i++)
	{
		arr[i].coords = (double *)calloc(dim, sizeof(double));
		arr[i].dimention = dim;
		if (arr[i].coords == NULL)
		{
			return 1;
		}
	}
	return 0;
}
void centroidsDestroy(Centroid *c, unsigned int k)
{
	pointDestroy(c, k);
}
void pointDestroy(Point *pArr, unsigned int k)
{
	coordsDestroy(pArr, k);
	free(pArr);
}
void coordsDestroy(Point *pArr, unsigned int k)
{
	size_t j = 0;
	if (pArr == NULL)
	{
		return;
	}
	for (; j < k; j++)
	{
		free(pArr[j].coords);
	}
}
void clusterPointDestroy(ClusterPoint *c, unsigned int n)
{
	size_t i = 0;
	if (c != NULL)
	{
		for (i = 0; i < n; i++)
		{
			coordsDestroy(&c[i].point, 1);
		}
	}
	free(c);
}
void cleanup()
{

	listDestroy(&dataPointsInput);
	clusterPointDestroy(dataPoints, n);
	centroidsDestroy(centroids, k);
	centroidsDestroy(updatedCentroids, k);
}
