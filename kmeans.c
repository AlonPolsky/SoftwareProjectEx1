#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
	Actual logic
*/
void initDataPointsWithStartingCluster(ClusterPoint*,Centroid*,unsigned int);
void updateDataPointCluster(ClusterPoint* data,const unsigned int dataSize,Centroid* centeroids,const unsigned int centeroidsSize);
int mmDistanceIndex(const Point* point,const Point* otherPoints,const unsigned int otherPointsSize,const int operator);
int minDistanceIndex(const Point* point,const Point* otherPoints,const unsigned int otherPointsSize);
int maxDistanceIndex(const Point* point,const Point* otherPoints,const unsigned int otherPointsSize);
double euclideanDistance (const Point* a, const Point* b);
void sumIntoPoint(Point* a, const Point* b);
void multipyPointByScalar(const Point* p,double scalar);
Centroid* getUpdatedCentroids(const ClusterPoint* clusterPoints,const unsigned int n ,const unsigned int k);
/*
	Error handeling
*/
void cleanup();
void centroidsDestroy(Centroid *,unsigned int k);
void pointDestroy(Point *,unsigned int k);
void listDestroy(LinkedList *);
void coordsDestroy(Point * pArr,unsigned int k);

/*
	User input
*/
ClusterPoint* dataPoints;
Centroid* centroids;
LinkedList dataPointsInput;
unsigned int n;
unsigned int k;
unsigned int iter = DEFUALT_ITER;

int main(int argc, char **argv)
{
	unsigned int itersCompleted = 0;
	size_t i = 0;
	unsigned int converged = 0;
	double maxDelta;
	double tempDelta;
	Centroid* updatedCentroids = NULL;



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

	dataPoints = (ClusterPoint*)malloc(sizeof(ClusterPoint) * n);
	centroids  = (Centroid*)	malloc(sizeof(Centroid) * k);
	if(dataPoints == NULL || centroids == NULL){
		ERROR_CLEANUP_AND_EXIT(GENERAL_ERROR_MSG);
	}
	parseDataPoints(&dataPointsInput,dataPoints);

	/*
		We finally have useable input, usable datapoint cluster array.
		init It according to step 1 in the algorithm
	*/
	initDataPointsWithStartingCluster(dataPoints,centroids,k);
	
	while( !converged  &&  itersCompleted < iter)
	{
		for (i = 0; i < n; i++)
		{
			
			/*
				Assign every xi to the closest cluster
			*/
			updateDataPointCluster(dataPoints,n,centroids,k);

		}


		
		updatedCentroids = getUpdatedCentroids(dataPoints,n,k);
		if(updatedCentroids == NULL)
		{
			ERROR_CLEANUP_AND_EXIT(GENERAL_ERROR_MSG);
		}




		maxDelta = 0;
		for (i = 0; i < k; i++)
		{
			tempDelta = euclideanDistance(&updatedCentroids[i],&centroids[i]);
			if(tempDelta>maxDelta)
			{
				maxDelta = tempDelta;
			}
		}
		if(maxDelta < EPSILON)
		{
			converged = 1;
		}
		centroidsDestroy(centroids,k);
		centroids = updatedCentroids;
		itersCompleted++;
	}
	
	
	
	
	/*
		Finished, Cleanup 
	*/
	cleanup();
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
		if(temp == NULL)
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
void initDataPointsWithStartingCluster(ClusterPoint* clusterPoints,Centroid* clusterCentroids,unsigned int clusterCount ){
	size_t i;
	for (i = 0; i < clusterCount; i++)
	{
		clusterCentroids[i] = clusterPoints[i].point;
		clusterPoints[i].belong = i + 1;
	}
	
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
void updateDataPointCluster(ClusterPoint* data,const unsigned int dataSize,Centroid* centeroids,const unsigned int centeroidsSize)
{
	size_t i;
	int minCluster;
	for ( i = 0; i < dataSize; i++)
	{
		minCluster = minDistanceIndex(&data[i].point,centeroids,centeroidsSize) + 1;
		data[i].belong = minCluster;
	}
	
}
int minDistanceIndex(const Point* point,const Point* otherPoints,const unsigned int otherPointsSize)
{
	return mmDistanceIndex(point,otherPoints,otherPointsSize,0);
}
int maxDistanceIndex(const Point* point,const Point* otherPoints,const unsigned int otherPointsSize)
{
	return mmDistanceIndex(point,otherPoints,otherPointsSize,1);
}
int mmDistanceIndex(const Point* point,const Point* otherPoints,const unsigned int otherPointsSize,const int operator)
{
	size_t i;
	unsigned int minIndex = 0;
	double minDistance = euclideanDistance(point,&otherPoints[0]);
	double tempDistance;
	for ( i = 1; i < otherPointsSize; i++)
	{
		tempDistance = euclideanDistance(point,&centroids[i]);
		if(operator==0 ? (tempDistance < minDistance):(tempDistance > minDistance))
		{
			minIndex = i;
			minDistance = tempDistance;
		}
	}
	return minIndex;
}

double euclideanDistance (const Point* a, const Point* b)
{
	double sum = 0;
	size_t i = 0;
	for(;i<a->dimention;i++)
	{
		sum+= pow((a->coords[i] - b->coords[i]),2);
	}
	return sqrt(sum);
}
Centroid* getUpdatedCentroids(const ClusterPoint* clusterPoints,const unsigned int n ,const unsigned int k){
	
	size_t i;
	Centroid* updatedCentroids = (Centroid*) malloc(sizeof(Centroid) * k);
	int* clusterSizeArr = (int*) calloc(k,sizeof(int));
	Point* clusterSumArr  =  (Point*) calloc(k,sizeof(Point));
	
	for (i = 0; i < k; i++)
	{
		clusterSumArr[i].coords = (double*)calloc(clusterPoints[i].point.dimention,sizeof(double));
		clusterSumArr[i].dimention = clusterPoints->point.dimention;
		if(clusterSumArr[i].coords == NULL)
		{
			return NULL;
		}
	}
	
	if(updatedCentroids == NULL || clusterSizeArr == NULL || clusterSumArr == NULL)
	{
		free(updatedCentroids);
		free(clusterSizeArr);
		free(clusterSumArr);
		return NULL;
	}


	for (i = 0; i < n; i++)
	{
		clusterSizeArr[clusterPoints[i].belong - 1] += 1;
		sumIntoPoint(&clusterSumArr[clusterPoints[i].belong - 1],&clusterPoints[i].point);  
	}

	for (i = 0; i < k; i++)
	{
		if(clusterSizeArr[i] == 0){
			return NULL;
		}
		multipyPointByScalar(&clusterSumArr[i],1/((double)clusterSizeArr[i]));
		updatedCentroids[i] = clusterSumArr[i];
	}
	
	free(clusterSizeArr);
	return updatedCentroids;
}
void sumIntoPoint(Point* sumInto, const Point* sumFrom){
	size_t i;
	for ( i = 0; i < sumInto->dimention; i++)
	{
		sumInto->coords[i] += sumFrom->coords[i];
	}
}
void multipyPointByScalar(const Point* p,double scalar){
	size_t i;
	for ( i = 0; i < p->dimention; i++)
	{
		p->coords[i] *= scalar;
	}
}

void centroidsDestroy(Centroid * c,unsigned int k)
{
	pointDestroy(c,k);
}
void pointDestroy(Point * pArr,unsigned int k)
{
	coordsDestroy(pArr,k);
	free(pArr);
	
}
void coordsDestroy(Point * pArr,unsigned int k)
{
	size_t i = 0;
	size_t j = 0;
	Point* temp;
	for (; j < k; j++)
	{
		temp = pArr;
		for (; i < pArr[0].dimention; i++)
		{
			free(pArr[i].coords);
		}
		pArr = temp+1;
	}
	
}
void clusterPointDestroy(ClusterPoint *c,unsigned int n)
{
	size_t i = 0;
	for ( i = 0; i < n; i++)
	{
		coordsDestroy(&c[i].point,1);
	}
	free(c);
}
void cleanup()
{
	listDestroy(&dataPointsInput);
	clusterPointDestroy(dataPoints,n);	
}
