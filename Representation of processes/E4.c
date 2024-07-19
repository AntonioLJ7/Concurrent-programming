*/ Develop a program that allows you to concurrently calculate the maximum of an array of elements of type int. To calculate the maximum, assume that
It has an array of N positions and M threads where N and M can be read from the keyboard or stored as constants. The values ​​contained in the
array are randomly initialized before thread creation. The developed solution will divide the array N into M fragments so that each
one of the threads will calculate the maximum of the array fragment that corresponds to it (it can be assumed for simplicity that N is a multiple of M). Later, a
Once all threads have finished calculating their local maximum, the main thread of the program will proceed to calculate the global maximum by examining the
local maximum of each of the threads */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PTHREADS 3
#define MAX_POSICIONES 9

int v[MAX_POSICIONES]; //Vector v[9]
int maximos[MAX_POSICIONES/MAX_PTHREADS]; //Vector maximo[3];

void *hebra(void *valor){

    long tid = *((long*)valor);
    long i = MAX_PTHREADS*tid; // 3*2, 3*1, 3*0

    //Valores que coje v[6],v[3],v[0] 
    int max = v[i];
    //Guardamos en max el valor maximos del vector v[i]
    //Valores que toma i = 6,3,0
    //MAX_PTHREADS*(tid+1) es para que lea el vector v[i] desde 8,5 y 2
    for(i;i<MAX_PTHREADS*(tid+1);i++){
        if(v[i]>max){
            max = v[i];
        }
    }
    //guardamos el  valor maximo de cada uno de los 3 vectores anteriores
    //maximos[2] = v[6-8],maximos[1] = v[3-5],maximos[0] = v[0-2]
    maximos[tid] = max;

}//void *hebra()

//Para inicializar el vector con valores 
void inicializar(int v[MAX_POSICIONES]){
    for(int i=0;i<MAX_POSICIONES;i++){
        v[i] = rand()%100;
    }
}//void inicializar()

int main(int argc,char *argv[]){

    inicializar(v);
    pthread_t pthreads[MAX_PTHREADS];
    long id[MAX_PTHREADS];
    int max=0;

    //Creacion de las 3 hebras
    for(long i=0;i<MAX_PTHREADS;i++){
        id[i] = i;
        long rc = pthread_create(&pthreads[i],NULL,hebra,&id[i]);
        if(rc){
            printf("ERROR\n");
        }
    }

    for(long i=0;i<MAX_PTHREADS;i++){
        pthread_join(pthreads[i],NULL);
    }

    //Para elegir el valor maximo del vector maximos[];
    max = maximos[0];
    for(int i=0;i<MAX_POSICIONES/MAX_PTHREADS;i++){
        if(maximos[i]>max){
            max = maximos[i];
        }
    }

    //Para visualizar los 9 valores del vector v[]
    for(int i=0;i<MAX_POSICIONES;i++){
        printf(" %d",v[i]);
    }

    printf("\n Maximo del vector: %d\n",max);

    pthread_exit(NULL);
}
