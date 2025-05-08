#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define NUM_THREADS 4

void *BusyWork(void *t)
{
    int i;
    long tid;
    double result = 0.0;
    tid = (long)t;

    sleep(2);
    printf("Thread %ld starting...\n", tid);
    for (i = 0; i < 1000000; i++)
    {
        result = result + sin(i) * tan(i);
    }
    printf("Thread %ld done. Result = %e \n", tid, result);
    pthread_exit((void *)t);
}

int main(int argc, char *argv[])
{
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    int rc;
    long t;
    void *status;

    /*Initialise and set the thread detached attribute*/
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /*create the threads and pass the attributes*/
    for (t = 0; t < NUM_THREADS; t++)
    {
        printf("Main: Creating Thread %ld\n", t);
        rc = pthread_create(&thread[t], &attr, BusyWork, (void *)t);

        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /*Free attributes and making main thread wait for other threads by joining*/
    pthread_attr_destroy(&attr);
    for (t = 0; t < NUM_THREADS; t++)
    {
        rc = pthread_join(thread[t], &status);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %ld having a status of %ld\n", t, (long)status);
    }
    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);
}