/******************************************************************************
 * FILE: non_mutex_example.c
 * DESCRIPTION:
 *   This program illustrates the use of mutex variables in a multi threaded
 *   programs.
 *   The main data is made accessible to all threads through a globally accessible
 *   data structure.
 *   The main thread waits for all the threads to complete their work, and then it
 *   ends
 * SOURCE: LLNL POSIX Threads Programming Tutorial
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    double *a;
    double *b;
    double sum;
    int veclen;
} DOTDATA;

#define VECLEN 100000
#define NUM_THREADS 4

DOTDATA dotstr;
pthread_t callThd[NUM_THREADS];
pthread_mutex_t mutexsum;

void *dotprod(void *arg)
{
    int start, end, i, len;
    long offset;
    double mysum, *x, *y;
    offset = (long)arg;

    len = dotstr.veclen;
    start = offset * len;
    end = start + len;
    x = dotstr.a;
    y = dotstr.b;

    mysum = 0;
    for (i = start; i < end; i++)
    {
        mysum += (x[i] * y[i]);
    }

    /* Lock the mutex prior to updating the value in the structure and unlock it after operation completes*/
    pthread_mutex_lock(&mutexsum);
    dotstr.sum += mysum;
    printf("Thread %ld did %d to %d:  mysum=%f global sum=%f\n", offset, start, end, mysum, dotstr.sum);
    pthread_mutex_unlock(&mutexsum);

    pthread_exit((void *)0);
}
int main(int argc, char *argv[])
{
    long i;
    double *a, *b;
    void *status;
    pthread_attr_t attr;

    a = (double *)malloc(NUM_THREADS * VECLEN * sizeof(double));
    b = (double *)malloc(NUM_THREADS * VECLEN * sizeof(double));

    for (i = 0; i < VECLEN * NUM_THREADS; i++)
    {
        a[i] = 1;
        b[i] = a[i];
    }
    dotstr.veclen = VECLEN;
    dotstr.a = a;
    dotstr.b = b;
    dotstr.sum = 0;

    pthread_mutex_init(&mutexsum, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&callThd[i], &attr, dotprod, (void *)i);
    }
    pthread_attr_destroy(&attr);

    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(callThd[i], &status);
    }

    printf("Sum = %f\n", dotstr.sum);
    free(a);
    free(b);
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
}