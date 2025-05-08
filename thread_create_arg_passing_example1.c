/******************************************************************************
* FILE: thread_create_arg_passing_example1.c
* DESCRIPTION:
*   A "hello world" Pthreads program which demonstrates one safe way
*   to pass arguments to threads during thread creation.
* AUTHOR: Blaise Barney
******************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 8

char *messages[NUM_THREADS];

void *PrintHello(void *threadid)
{
    long taskid;

    sleep(1);
    taskid = (long)threadid;
    printf("Thread %ld: %s\n", taskid, messages[taskid]);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    long taskids[NUM_THREADS];
    int rc, t;

    messages[0] = "English: Hello World!";
    messages[1] = "French: Bonjour, le monde!";
    messages[2] = "Spanish: Hola al mundo";
    messages[3] = "Klingon: Nuq neH!";
    messages[4] = "German: Guten Tag, Welt!";
    messages[5] = "Russian: Zdravstvuy, mir!";
    messages[6] = "Japan: Sekai e konnichiwa!";
    messages[7] = "Latin: Orbis, te saluto!";

    for (t = 0; t < NUM_THREADS; t++)
    {   
        // creating a unique data structure that makes sure no other thread can change the value of this argument
        taskids[t] = t;
        printf("In main: Creating thread %d\n", t);
        rc = pthread_create(&threads[t], NULL, PrintHello, (void *)taskids[t]);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    /* Last thing that main() should do */
    pthread_exit(NULL);
}
