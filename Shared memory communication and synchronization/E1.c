/* Implement a C program that allows you to calculate terms of the Fibonacci sequence.
N threads will be available to calculate up to the Nth term of said sequence, so
so that the thread with ith identifier must calculate the ith term using the
values ​​of strands i-1 and i-2. Implement a solution based on hot standby. The strands
They must wait until the terms prior to theirs are calculated. Consider the
terms 0 and 1 as special cases. */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100

long sucecion[N];

void inicializar_vector(long v[N]);

void *hebra(void *argumento){
    long tid;

    tid = *((long*)argumento);

    if(tid > 1){
        while((sucecion[tid-1] == 0)||(sucecion[tid-2]==0));
        sucecion[tid]=sucecion[tid-1]+sucecion[tid-2];
    }else{
        sucecion[tid] = 1;
    }

}

int main(int argc,char *argv[]){

    inicializar_vector(sucecion);

    pthread_t pthreads[N];
    long rc;
    long id[N];

    for(long i=0;i<N;i++){

        id[i] = i; 
        rc = pthread_create(&pthreads[i],NULL,hebra,&id[i]);

        if(rc){
            printf("ERROR");
            exit(-1);
        }

    } 

    for(long i=0;i<N;i++){
        pthread_join(pthreads[i],NULL);
    }

    for(long i=0;i<N;i++){
        printf("   %d",sucecion[i]);
    }

    pthread_exit(NULL);
}

void inicializar_vector(long v[N]){
    for(long i=0;i<N;i++){
        v[i] = 0;
    }
}