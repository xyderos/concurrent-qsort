#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

//concurrent version of quicksort definition
void qsort_c(int*,int,int,int);

static inline void swap(int*,int*);

static inline void swap(int* a, int* b){

    int t = *a;

    *a = *b;

    *b = t;
}

double read_timer() {

    static bool initialized = false;

    static struct timeval start;

    struct timeval end;
    if( !initialized ){

        gettimeofday( &start, NULL );

        initialized = true;
    }

    gettimeofday( &end, NULL );

    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time;

//cast to void* then re cast it back as a struct so it can be passed as an argumnt for the threads
typedef struct __starter{

    int *subArray;

    int l;

    int h;

    int depth;
} args;

int partition (int *arr, int low, int high, int pivot){

    int pivotValue = arr[pivot];

    swap(&arr[pivot],&arr[high]);

    int s = low;

    for (int i = low; i <high; i++) if (arr[i] <= pivotValue) swap(&arr[i], &arr[s++]);

    swap(&arr[s], &arr[high]);

    return s;
}

void qsort_seq(int *arr, int l, int h){

    if (l < h){

        int pivotPos = l + (h - l) / 2;

        pivotPos= partition(arr, l, h, pivotPos);

        qsort_seq(arr, l, pivotPos - 1);

        qsort_seq(arr, pivotPos + 1, h);
    }
}

void* Worker(void * initialValues){

    args* parameters = initialValues;

    qsort_c(parameters->subArray, parameters->l, parameters->h, parameters->depth);

    return NULL;
}

void qsort_c(int *arr, int l, int h, int depth){

    if (l < h){

        int pivotPos = l + (h - l) / 2;

        pivotPos = partition(arr, l, h, pivotPos);

        pthread_t thread;

        if (depth > 0){

            args params = {arr, l, pivotPos - 1, depth};
            
	    pthread_create(&thread, NULL, Worker, &params);

            qsort_c(arr, pivotPos + 1, h, depth);


            pthread_join(thread, NULL);

        } else{
            qsort_seq(arr, l, pivotPos - 1);

            qsort_seq(arr, pivotPos + 1, h);
        }
    }
}

int main(int argc, char **argv){

    int depthOfThreadCreation = 0;

    if (argc > 1) depthOfThreadCreation =atoi(argv[1]);

    int size = 8;

    if (argc > 2) size = atoi(argv[2]);

    int *arrayElements = malloc(size*  sizeof(long));

    for (int i=0 ; i<size ; i++) arrayElements[i] = rand()%999;

    start_time = read_timer();

    qsort_c(arrayElements, 0, size - 1, depthOfThreadCreation);

    end_time = read_timer();

    printf("The execution time was:  %lf secs\n", end_time - start_time);
    
    free(arrayElements);
    return 0;
}
