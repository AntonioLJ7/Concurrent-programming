/* Implement the producer-consumer problem in C with a circular buffer of N
positions.
– There is a single strand representing the producer that will produce elements of
consecutive integer type.
– There is a bounded circular buffer shared by the producer and the consumer to
produce and consume the elements where they will have the following attributes:
• elem: array with the buffer elements.
• p: indicates the position by which it is produced.
• c: indicates the position through which it is consumed.
• nelem: number of valid elements in the buffer.
– There is a single consumer that will display the elements on the screen
deposited in the buffer. */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10

int elem[N];
int nelem = 0;
int turno = 1;
int f[2];

void inicializar_vector(long v[N]);

void *productor(void *producto){
    int i=0;

    while(1){
        f[0] = 1;
        turno = 1;

        while(nelem == N){ //Si llega al maximo se vuelve producto a 0
            if(p==N){
                p=0;
            }
        }

        elem[p] = i;
        printf("produzco  %n\n",elem[p]);
        p++;
        i++;
        while(f[1]==1 && turno==1);
        nelem++;
        f[0] = 0;
    }
}

void *consumidor(void *consumo){
    while(1){
        f[1] = 1;
        turno = 0;
        while(nelem == N){
            if(c == N){
                c=0;
            }
        }
        printf("Consume  %n\n",elem[c]);
        c++;
        while(f[0]==1 && turno==0);
        nelem--;
        f[1] = 0;
    }
}

int main(int argc,char *argv[]){

    pthread_t threads[2];


    long rc1 = pthread_create(&threads[0],NULL,productor,NULL);
    if(rc1){
        printf("ERROR PR0DUCTOR");
    }
    
    long rc2 = pthread_create(&threads[1],NULL,consumidor,NULL);
    if(rc2){
        printf("ERROR CONSUMIDOR");
    }

    for(long i=0;i<2;i++){
        pthread_join(threads[i],NULL);
    }

}

void inicializar_vector(long v[N]){
    for(long i=0;i<N;i++){
        v[i] = 0;
    }
}