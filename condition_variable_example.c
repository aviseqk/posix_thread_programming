/******************************************************************************
 * FILE: condition_variable_example.c
 * DESCRIPTION:
 * This program demonstrates the use of several Pthread condition variable
 * routines wherein they are used for synchronisation
 * The main routine creates three threads
 * Two of the threads perform work and update a global count variable
 * The third thread waits until the count variable reaches a specific value
 * SOURCE: LLNL POSIX Threads Programming Tutorial
 ******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3
#define TCOUNT 10
#define COUNT_LIMIT 12

int count = 0;
/* Declaring the mutex and condition variable objects*/
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

void *inc_count(void *t)
{
    int i;
    long my_id = (long)t;

    for (i = 0; i < TCOUNT; i++)
    {
        pthread_mutex_lock(&count_mutex);
        count++;

        /* Check the value of count and signal waiting thread when condition is reached */
        /* Note that this occurs when the mutex is locked*/
        if (count == COUNT_LIMIT)
        {
            printf("inc_count(): thread %ld, count = %d -- threshold reached\n", my_id, count);
            pthread_cond_signal(&count_threshold_cv);
            printf("Just sent signal\n");
        }
        printf("inc_count(): thread %ld, count = %d -- unlocking mutex\n", my_id, count);
        pthread_mutex_unlock(&count_mutex);
        // Do some work so threads can alternate on mutex lock
        sleep(1);
    }
    pthread_exit(NULL);
}

void *watch_count(void *t)
{
    long my_id = (long)t;
    printf("watch_count(): Starting thread watch on %ld\n", my_id);

    /* Lock mutex and wait for signal. Note that the pthread_cond_wait routine will automatically
    unlock mutex while it waits. */
    pthread_mutex_lock(&count_mutex);

    while (count < COUNT_LIMIT)
    {
        printf("watch_count(): thread %ld Count= %d. Going into wait...\n", my_id, count);
        pthread_cond_wait(&count_threshold_cv, &count_mutex);
        printf("watch_count(): thread %ld Condition signal received. Count= %d\n", my_id, count);
    }
    printf("watch_count(): thread %ld Updating the value of count...\n", my_id);
    count += 125;
    printf("watch_count(): thread %ld count now = %d.\n", my_id, count);
    printf("watch_count(): thread %ld Unlocking mutex.\n", my_id);
    pthread_mutex_unlock(&count_mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int i, rc;
    long t1 = 1, t2 = 2, t3 = 3;
    pthread_t threads[3];
    pthread_attr_t attr;

    /* Initialize the mutex and condition variable objects*/
    pthread_mutex_init(&count_mutex, NULL);
    pthread_cond_init(&count_threshold_cv, NULL);

    /* explicitly create all the threads in a JOINABLE state*/
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], &attr, watch_count, (void *)t1);
    pthread_create(&threads[1], &attr, inc_count, (void *)t2);
    pthread_create(&threads[2], &attr, inc_count, (void *)t3);

    /* Wait for all the threads to complete*/
    for ( i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("main(): Waited and joined with %d threads. Final value of count = %d. Done.", 
        NUM_THREADS, count);
    
    /* Cleanup and exit */
    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_threshold_cv);
    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}