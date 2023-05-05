/*
 * sum_parallel.c
 * 
 * Originally from 18447 HW5
 */

#include <pthread.h>
#include <stdio.h>

#define THREAD_COUNT (10)
#define ELEMENT_COUNT (100)

// shared global variables
double sum=0;               // final sum to be produced by the program
double A[ELEMENT_COUNT];    // array to be summed in sumParallel
pthread_mutex_t mutex;

void *sum_parallel(void *arg) {
    long tid, i; 
    
    tid = (long)arg;

    for (i = 0; i < (ELEMENT_COUNT/THREAD_COUNT); i++) {
        pthread_mutex_lock(&mutex);
        sum += A[tid * ELEMENT_COUNT/THREAD_COUNT + i];
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[THREAD_COUNT];
    double correct_sum = 0;
    long which;
    int i;

    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < ELEMENT_COUNT; i++) {
        A[i] = i;
        correct_sum += A[i];
    }

    for(which = 0; which < THREAD_COUNT; which++ ) {
        pthread_create(&threads[which], NULL, sum_parallel, (void *)which);
    }

    for(which = 0; which < THREAD_COUNT; which++ ) {
        pthread_join(threads[which], NULL);
    }

    printf("correct_sum \t= %lf\n", correct_sum);
    printf("sum \t\t= %lf\n", sum);

    return 0;
}