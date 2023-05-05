/*
 * axpy.c
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THREAD_COUNT (10)
#define ELEMENT_COUNT (10)
#define ELEMENT_MAX (100.0)

// shared global variables
double a;
double X[ELEMENT_COUNT];    // array to be summed in sumParallel
double Y[ELEMENT_COUNT];    // array to be summed in sumParallel
pthread_mutex_t mutex;

void *axpy(void *arg) {
    long tid, start, idx, i; 
    
    tid = (long)arg;

    start = tid * ELEMENT_COUNT/THREAD_COUNT;
    for (i = 0; i < (ELEMENT_COUNT/THREAD_COUNT); i++) {
        idx = start + i;
        Y[idx] = a * X[idx] + Y[idx];
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[THREAD_COUNT];
    double correct_sum = 0;
    long which;
    int i;

    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);
    
    // Initialize seed
    srand((unsigned int)time(NULL));

    // Randomize vector and scalar values
    a = ((double)(rand()) / (double)(RAND_MAX)) * (float)(ELEMENT_MAX);
    for (i = 0; i < ELEMENT_COUNT; i++) {
        X[i] = ((double)(rand()) / (double)(RAND_MAX)) * (float)(ELEMENT_MAX);
        Y[i] = ((double)(rand()) / (double)(RAND_MAX)) * (float)(ELEMENT_MAX);
    }

    printf("a = %0.2lf\n", a);

    printf("X = [");
    for (i = 0; i < ELEMENT_COUNT; i++) {
        printf("%0.2lf", X[i]);
        if (i != ELEMENT_COUNT-1) printf(", ");
    }
    printf("]\n");

    printf("Y = [");
    for (i = 0; i < ELEMENT_COUNT; i++) {
        printf("%0.2lf", Y[i]);
        if (i != ELEMENT_COUNT-1) printf(", ");
    }
    printf("]\n");
    

    // Spawn threads
    for(which = 0; which < THREAD_COUNT; which++ ) {
        pthread_create(&threads[which], NULL, axpy, (void *)which);
    }

    // Join threads
    for(which = 0; which < THREAD_COUNT; which++ ) {
        pthread_join(threads[which], NULL);
    }

    printf("Performed Y <- aX+Y\n");

    printf("Y = [");
    for (i = 0; i < ELEMENT_COUNT; i++) {
        printf("%0.2lf", Y[i]);
        if (i != ELEMENT_COUNT-1) printf(", ");
    }
    printf("]\n");

    return 0;
}