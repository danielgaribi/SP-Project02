#include <Python.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define true 1
#define false 0
#define DEFAULT_MAX_ITER 200

struct node
{
    double* point;
    struct node *next;
};

struct linked_list
{
    struct node *head;
    struct node *tail;
    int length;
};

struct node_double
{
    double value;
    struct node_double *next;
};

struct linked_list_double
{
    struct node_double *head;
    struct node_double *tail;
};

typedef struct node node;
typedef struct linked_list linked_list;
typedef struct node_double node_double;
typedef struct linked_list_double linked_list_double;

void kmean(linked_list *pointsArray, double **centroids, int k, int max_iter, int d);
int readArgs(int argc, char *argv[], int* k, int* max_iter);
int readPointsArray(linked_list* pointsList);
void addToList(linked_list* list, double* point);
void addToListDouble(linked_list_double* list, double value);
double* convertDoubleListToArray(linked_list_double* list, int d);
void freeList(linked_list* list, int isDeletePoint);
void freeListDouble(linked_list_double* list);
void freeNode(node* n, int isDeletePoint);
void freeNodeDouble(node_double* n);
double computeDist(double* point1, double* point2, int d);
int isPointsEquel(double* point1, double* point2, int d);
double* copy_point(double* point, int d);
int computeCluster(int k, int d, double** centroids, linked_list* pointsList);
int computeNewCentroids(linked_list** clusters, double** centroids, int k, int d);
void printOutput(double** centroids, int k, int d);
void freeDouble2DArray(double **centroids, int k);
static PyObject *fit( PyObject *self, PyObject *args );

int main(int argc, char *argv[]) {
    
    return 0;
}

int readArgs(int argc, char *argv[], int* k, int* max_iter) {
    *k = atoi(argv[1]);
    if (*k <= 0) {
        printf("K is not a valid integer, exits...\n");
        return false;
    }

    *max_iter = argc == 3 ? atoi(argv[2]) : DEFAULT_MAX_ITER;
    if (*max_iter <= 0) {
        printf("max_iter is not a valid integer, exits...\n");
        return false;
    }

    #ifdef DEBUG_INPUT
    printf("K: %d \nmax_iter: %d\n", *k, *max_iter);
    #endif  

    return true;
}

int readPointsArray(linked_list* pointsList) {
    int d = 0, i = 0;
    double value;
    char c;
    double *point;
    linked_list_double *pointA = (linked_list_double*)malloc(sizeof(linked_list)); 
    assert(pointA != NULL);
    while(scanf("%lf%c", &value, &c) == 2) {
        d++;
        addToListDouble(pointA, value);
        if(c == '\n') {
            break;
        }
    }
    point = (double*)calloc(d, sizeof(double)); 
    assert(point != NULL);
    addToList(pointsList, convertDoubleListToArray(pointA, d));
    freeListDouble(pointA);

    while (scanf("%lf%c", &value, &c) > 0) {
        point[i] = value;
        i++;
        if(i == d) {
            i = 0;
            addToList(pointsList, point);
            point = (double*)calloc(d, sizeof(double)); 
            assert(point != NULL);
        }
    } 

    #ifdef DEBUG_INPUT
    int index = 0;
    for (node *n = pointsList -> head; n != NULL; n = n -> next) {
        printf("Point %d: ", index);
        for (int j = 0; j < d; j++) {
            printf("%lf, ", (n -> point)[j]);
        }
        printf("\n");
        index++;
    }
    #endif

    return d;
}

void addToList(linked_list* list, double* point) {
    node *n = (node*)malloc(sizeof(node)); 
    assert(n != NULL);
    n -> point = point;
    n -> next = NULL;
    (list->length)++;
    if(list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    } else {
        list -> tail -> next = n;
        list -> tail = n;
    }
}

void addToListDouble(linked_list_double* list, double value) {
    node_double *n = (node_double*)malloc(sizeof(node_double));
    assert(n != NULL);
    n -> value = value;
    n -> next = NULL;
    if(list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    } else {
        list -> tail -> next = n;
        list -> tail = n;
    }
}

double* convertDoubleListToArray(linked_list_double* list, int d) {
    node_double *n = list -> head;
    int i;
    double *point = (double*)calloc(d, sizeof(double));
    assert(point != NULL);
    for(i = 0; n != NULL; n = n -> next, i++) {
        point[i] = n -> value;
    }
    return point;
}

void freeList(linked_list* list, int isDeletePoint) { 
    freeNode(list -> head, isDeletePoint);
    free(list);
}

void freeNode(node* n, int isDeletePoint) {
    if (n != NULL) {
        freeNode(n -> next, isDeletePoint);
        if(isDeletePoint == true){
            free(n -> point);
        }
        free(n);
    }
}

void freeListDouble(linked_list_double* list) {
    freeNodeDouble(list -> head);
    free(list);
}

void freeNodeDouble(node_double* n) {
    if (n != NULL) {
        freeNodeDouble(n -> next);
        free(n);
    }
}

void freeDouble2DArray(double **centroids, int k) {
    int i;
    for (i = 0; i < k; i++) {
        free(centroids[i]);
    }
    free(centroids);
}

void kmean(linked_list *pointsArray, double **centroids, int k, int max_iter, int d) {
    int iter, isChanged;

    for (iter = 0; iter < max_iter; iter++) {
        isChanged = computeCluster(k, d, centroids, pointsArray);
        if (!isChanged) {
            break;
        }
    }
    printOutput(centroids, k, d);
    freeDouble2DArray(centroids, k);
}

double* copy_point(double* point, int d) {
    int i;
    double* new_point = calloc(d, sizeof(double));
    assert(new_point != NULL);
    for (i = 0; i < d; i++) {
        new_point[i] = point[i];
    }
    return new_point;
}

int computeCluster(int k, int d, double** centroids, linked_list* pointsList) {
    double minDist, dist;
    int minIndex, isChanged, i;
    node* n;
    linked_list** clusters = (linked_list**)calloc(k, sizeof(linked_list*)); 
    assert(clusters != NULL);

    for(i = 0; i < k; i++) {
        clusters[i] = (linked_list*)calloc(1, sizeof(linked_list));
        assert(clusters[i] != NULL);
    }

    for(n = pointsList -> head; n != NULL; n = n -> next ) {
        minIndex = 0;
        minDist = computeDist(centroids[0], n -> point, d);
        for (i = 1; i < k; i++) {
            
            dist = computeDist(centroids[i], n -> point, d);
            if (dist < minDist) {
                minDist = dist;
                minIndex = i;
            }
        }
        addToList(clusters[minIndex], n -> point);
    }
    isChanged = computeNewCentroids(clusters, centroids, k, d);
    for(i = 0; i < k; i++){
        freeList(clusters[i], false);
    }
    free(clusters);
    return isChanged;
}

int computeNewCentroids(linked_list** clusters, double** centroids, int k, int d) {
    double* oldCentroid, *newCentroid;
    int i, j, t, isChanged = false;
    node* n;
    for (i = 0; i < k; i++) {
        oldCentroid = copy_point(centroids[i], d);
        newCentroid = centroids[i];
        j = 0;
        for (n = (clusters[i]) -> head; n != NULL; n = n-> next) {
            for(t = 0; t < d; t++) {
                newCentroid[t] = (newCentroid[t] * j + (n -> point)[t]) / (j + 1);
            }
            j++;
        }
        if (isPointsEquel(oldCentroid, newCentroid, d) == false){
            isChanged = true;
        }
        free(oldCentroid);
    }
    return isChanged;
}

double computeDist(double* point1, double* point2, int d) {
    double dist = 0, tmp = 0; 
    int i;
    for (i = 0; i < d; i++) {
        tmp = (point1[i] - point2[i]);
        dist += tmp * tmp;
    }
    return dist;
}

int isPointsEquel(double* point1, double* point2, int d) {
    int i;
    for (i = 0; i < d; i++) {
        if (point1[i] != point2[i]) {
            return false;
        }
    }
    return true;
}

void printOutput(double** centroids, int k, int d) {
    int i;
    int j;
    for (i = 0; i < k; i++) {
        for (j = 0; j < d; j++) {
            printf("%.4f", centroids[i][j]);
            if(j != d - 1){
                printf(",");
            }
        }
        printf("\n");
    }
}

static double **convertPyListToCentroidsArray(PyObject *cetroidsList, int d) {
    PyObject *centroidItem, *pointItem;
    double **centroids, *new_point;
    int cetroidsLength;

    cetroidsLength = PyObject_Length(cetroidsList); 
    assert(cetroidsLength == -1); // PyObject_Length return -1 for error

    // insert centroids to double array
    centroids = calloc(cetroidsLength, sizeof(double*));
    assert(centroids != NULL);

    for (int i = 0; i < cetroidsLength; i++) {        
        centroidItem = PyList_GetItem(cetroidsList, i);
        assert(centroidItem =! NULL);
        new_point = calloc(d, sizeof(double));
        for (int j = 0; j < d; j++) {
            pointItem = PyList_GetItem(centroidItem, j);
            assert(pointItem =! NULL);
            assert(PyFloat_Check(pointItem));
            new_point[j] = PyFloat_AsDouble(pointItem);
        }
        centroids[i] = new_point;
    }
    return centroids;
}

static linked_list *convertPyListToPointsLinkList(PyObject *datapointsList, int d) {
    PyObject *datapointsItem, *pointItem;
    linked_list* pointsList;
    double *new_point;
    int datapointsLength;

    datapointsLength = PyObject_Length(datapointsList);
    assert(datapointsLength == -1); // PyObject_Length return -1 for error
    pointsList = (linked_list*)calloc(1,sizeof(linked_list));
    pointsList->length = 0;

    // insert datapoints to linked list
    for (int i = 0; i < datapointsLength; i++) {        
        datapointsItem = PyList_GetItem(datapointsList, i);
        assert(datapointsItem =! NULL);
        new_point = calloc(d, sizeof(double)); 
        for (int j = 0; j < d; j++) {
            pointItem = PyList_GetItem(datapointsItem, j);
            assert(pointItem =! NULL);
            assert(PyFloat_Check(pointItem));
            new_point[j] = PyFloat_AsDouble(pointItem);
        }
        addToList(pointsList, new_point);
    }
    return pointsList;
}

static PyObject *fit( PyObject *self, PyObject *args ){
    PyObject *datapointsList, *cetroidsList;
    int k, max_iter, d;
    linked_list* pointsList;
    double **centroids;
    assert(pointsList != NULL);
    
    if (!PyArg_ParseTuple(args, "iiiOO", &k, &d, &max_iter, &datapointsList, &cetroidsList))
        return NULL;

    centroids = convertPyListToCentroidsArray(cetroidsList, d);
    pointsList = convertPyListToPointsLinkList(datapointsList, d);
    
    kmean(pointsList, centroids, k, max_iter, d);
    freeList(pointsList, true);

    Py_RETURN_NONE;
}

static PyMethodDef DemoLib_FunctionsTable[] = {
   { "fit", fit, METH_VARARGS, NULL },
   { NULL, NULL, 0, NULL }
};

// modules definition
static struct PyModuleDef DemoLib_Module = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",     // name of module exposed to Python
    "mykmeanssp doc", // module documentation
    -1,
    DemoLib_FunctionsTable
};

PyMODINIT_FUNC PyInit_mykmeanssp(void) {
    return PyModule_Create(&DemoLib_Module);
}