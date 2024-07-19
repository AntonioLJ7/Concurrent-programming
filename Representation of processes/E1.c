/* Make a program with 3 threads, each of which writes to the screen
several times (value passed as parameter) the character indicated. */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_THREADS 3

void *hebra(void *pthreadc){
    char c;
     c = *((char*)pthreadc);

     for(int i=0;i<10;i++){
         printf("letra %c\n",c);
     }
     pthread_exit(NULL);
}

int main(int argc,char *argv[]){

    pthread_t threads[NUM_THREADS];
    char cs[] = {'a','b','h'};
    int rc;

    for(int i=0;i<NUM_THREADS;i++){
        rc = pthread_create(&threads[i],NULL,hebra,&cs[i]);

        if(rc){
            printf("Error %d\n",rc);
            exit(-1);
        }
    }
    pthread_exit(NULL);
}
