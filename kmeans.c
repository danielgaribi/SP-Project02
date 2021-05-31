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

typedef struct node node;
typedef struct linked_list linked_list;

double** kmean(linked_list *pointsArray, double **centroids, int k, int max_iter, int d);
void addToList(linked_list* list, double* point);
void freeList(linked_list* list, int isDeletePoint);
void freeNode(node* n, int isDeletePoint);
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

void freeDouble2DArray(double **centroids, int k) {
    int i;
    for (i = 0; i < k; i++) {
        free(centroids[i]);
    }
    free(centroids);
}

double** kmean(linked_list *pointsArray, double **centroids, int k, int max_iter, int d) {
    int iter, isChanged;

    for (iter = 0; iter < max_iter; iter++) {
        isChanged = computeCluster(k, d, centroids, pointsArray);
        if (!isChanged) {
            break;
        }
    }
    return centroids;
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

static double **convertPyListToCentroidsArray(PyObject *cetroidsList, int d) {
    PyObject *centroidItem, *pointItem;
    double **centroids, *new_point;
    int cetroidsLength, i, j;

    cetroidsLength = PyObject_Length(cetroidsList); 
    assert(cetroidsLength == -1); // PyObject_Length return -1 for error

    // insert centroids to double array
    centroids = calloc(cetroidsLength, sizeof(double*));
    assert(centroids != NULL);

    for (i = 0; i < cetroidsLength; i++) {        
        centroidItem = PyList_GetItem(cetroidsList, i);
        assert(centroidItem =! NULL);
        new_point = calloc(d, sizeof(double));
        for (j = 0; j < d; j++) {
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
    int datapointsLength, i, j;

    datapointsLength = PyObject_Length(datapointsList);
    assert(datapointsLength == -1); // PyObject_Length return -1 for error
    pointsList = (linked_list*)calloc(1,sizeof(linked_list));
    pointsList->length = 0;

    // insert datapoints to linked list
    for (i = 0; i < datapointsLength; i++) {        
        datapointsItem = PyList_GetItem(datapointsList, i);
        assert(datapointsItem =! NULL);
        new_point = calloc(d, sizeof(double)); 
        for (j = 0; j < d; j++) {
            pointItem = PyList_GetItem(datapointsItem, j);
            assert(pointItem =! NULL);
            assert(PyFloat_Check(pointItem));
            new_point[j] = PyFloat_AsDouble(pointItem);
        }
        addToList(pointsList, new_point);
    }
    return pointsList;
}

PyObject *convertCentroidsArrayToPyList(double** array, int k, int d) {
    PyObject * lst = PyList_New(k), *innerLst;
    int i ,j;
    for (i = 0; i < k ; i++) {
        innerLst = PyList_New(d);
        for (j = 0; j < d; j++) {
            PyList_SET_ITEM(innerLst, j, Py_BuildValue("d", array[i][j]));
        }
        PyList_SET_ITEM(lst, i, innerLst);
    }
    freeDouble2DArray(array, k);
    return lst;
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
    
    centroids = kmean(pointsList, centroids, k, max_iter, d);
    freeList(pointsList, true);

    return convertCentroidsArrayToPyList(centroids, k, d);
}

static PyMethodDef Mykmeanssp_FunctionsTable[] = {
   { "fit", fit, METH_VARARGS, NULL },
   { NULL, NULL, 0, NULL }
};

// modules definition
static struct PyModuleDef Mykmeanssp_Module = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",     // name of module exposed to Python
    "mykmeanssp doc", // module documentation
    -1,
    Mykmeanssp_FunctionsTable
};

PyMODINIT_FUNC PyInit_mykmeanssp(void) {
    return PyModule_Create(&Mykmeanssp_Module);
}