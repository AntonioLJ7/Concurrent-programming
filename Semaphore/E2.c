/* In an assembly line there is a robot in charge of placing products of 3 types
different (1, 2 or 3) on the assembly line. Other robots remove products from the chain
assembly to carry out its packaging, taking into account that they are specialized in a
only type of product (1, 2 or 3), ignoring those that are not of its type. Finally, you want
keep track of the total number of packaged products (regardless of their type).
Model the system described using traffic lights with the following indications:
– Model each robot as a strand (1 setter and 3 packers, one for each
kind of product).
– The products are placed one by one in the chain, and only in positions
free (it can be considered that a maximum N of elements fit in the assembly line).
If there are no free positions, the placing robot will have to wait until some product
be removed from the chain.
– Packaging robots specialize in one type of product (1, 2 or 3) in
initialization time.
– Packing robots check if there is any item of its type in the
chain ignoring products that are not of its type. If there is any product of your type
removed from the chain (only 1 product at a time) and the position is free to place new ones
products.
– Packing robots of different types can work at the same time.
– Both the setter and the packers never finish.
– Every time a packaging robot processes a product, the total count of
packaged products must be enlarged and displayed on the screen */

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define N 6

sem_t emp[3];
sem_t colocador;
sem_t mutex;

int buffer[N];
int cuenta = 0;

int buscaHueco();
int buscaNumero(int n);

void* Colocador(void* args){

    while(1){

        sem_wait(&colocador);
        sem_wait(&mutex);
        
        int h = buscaHueco(); //Buscamos un hueco libre
        int a = rand()%3; // Generamos producto

        buffer[h] = a; //El producto lo introducimos 

        sem_post(&mutex);
        sem_post(&emp[a]); //Elige empaquetador dependiendo del producto
    }

}

void* Empaquetadores(void* args){

    int id = *(int *)args;

    while(1){

        sem_wait(&emp[id]);
        sem_wait(&mutex);

        int p = buscaNumero(id);//Buscamo el producto a sacar
        buffer[p] = -1;//Producto empaquetado(lo eliminamos el buffer poniendo a -1)
        cuenta++;
        printf("Cuenta: %d\n",cuenta);
        fflush(stdout);

        sem_post(&mutex);
        sem_post(&colocador);
    }

}


int main(){

    pthread_t hemp[3],coloc;
    int ids[3];

    sem_init(&emp[0],0,0);
    sem_init(&emp[1],0,0);
    sem_init(&emp[2],0,0);
    sem_init(&colocador,0,N);
    sem_init(&mutex,0,1);

    for(int i=0;i<N;i++){
        buffer[i] = -1;
    }

    for(int i=0;i<3;i++){
        ids[i] = i;
        pthread_create(&hemp[i],NULL,Empaquetadores,&ids[i]);
    }
    pthread_create(&coloc,NULL,Colocador,NULL);

    for(int i=0;i<3;i++){
        pthread_join(hemp[i],NULL);
    }
    pthread_join(coloc,NULL);

}

//Rellenamos el buffer con -1(vacio)
int buscaHueco(){
    return buscaNumero(-1);
}
//Buscamos en el buffer hasta que encontremos -1 (posicion vacia)
int buscaNumero(int n){
    int i=0;
    while(buffer[i] != n)i++;
    return i;
}