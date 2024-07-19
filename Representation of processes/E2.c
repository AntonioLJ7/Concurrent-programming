/* Suppose you have a global variable that will be shared by several
threads and that functions or procedures are defined to consult or modify the
value of that variable. Make a program that creates 2 threads and increments
each of them 100 times the value of v. Show from program thread
main the final value of v */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_PTHREAD 2   

int v = 0;

void *hebra(void *argumento){

    long c;
    c = *((long*)argumento);
    for(int i=0;i<100;i++){
        v++;
    }
    printf("%d\n",v);
}

int main(int agc,char *argv[]){

    pthread_t pthreads[NUM_PTHREAD];
    long id[NUM_PTHREAD];

    for(long i=0;i<NUM_PTHREAD;i++){
        id[i] = i;
        long rc = pthread_create(&pthreads[i],NULL,hebra,&id[i]);
        if(rc){
            printf("ERROR %i\n",i);
            exit(-1);
        }
    }

    for(long i=0;i<NUM_PTHREAD;i++){
        pthread_join(pthreads[i],NULL);
    }

    pthread_exit(NULL);
}