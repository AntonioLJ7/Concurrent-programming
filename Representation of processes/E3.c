/* Using the SharedVariable, create a program where we have
a thread that modifies SharedVariable values ​​from 0 to 99
using a set operation. We also have another strand, which using
get should display on the screen all the changes that occur in the
SharedVariable (knowing that 100 values ​​will be generated) */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_PTHREAD 2

int v = 0;
int x = 1;

void *set(void *argumento){
    long tid;

    tid = *((long*)argumento);

    for(long i=0;i<100;i++){
        while(!x);
        v++;
        x = 0;
    }
}

void *get(void *argumento){
    long tid;
    tid = *((long*)argumento);

    for(long i=0;i<100;i++){
        while(x);
        printf("\n%d",v);
        x = 1;
    }
}

int main(int agc,char *agrv[]){

    pthread_t pthreads[NUM_PTHREAD];
    long id[NUM_PTHREAD];

    long rc = pthread_create(&pthreads[0],NULL,set,&id[0]);
    if(rc){
        printf("ERROR");
    }

    rc = pthread_create(&pthreads[1],NULL,get,&id[1]);
    if(rc){
        printf("ERROR");
    }

    for(long i=0;i<NUM_PTHREAD;i++){
        pthread_join(pthreads[i],NULL);
    }

    printf("%d\n",v);

    pthread_exit(NULL);
}