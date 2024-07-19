/* In an industrial system there are three sensors that perform temperature level measurements.
temperature, humidity and light respectively. When measurements have been collected from all three
sensors, there is a “worker” device in charge of performing certain tasks according to the
measurements made.
The device cannot begin performing its tasks until measurements have been collected
of the three sensors, and the sensors cannot take measurements again until the
device finishes its tasks. The process is repeated indefinitely so that
When the device finishes its tasks, it will wait again for measurements of the three
sensors. */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

sem_t s[3];
sem_t strab;
int med[3];

void* Sensor(void* args){
    int id = *(long*)args;

    while(1){

        sem_wait(&s[id]);

        printf("Comienza ha medir\n");
        fflush(stdout);

        med[id] = rand();
        sleep(rand()%5);
        

        printf("Termino de medir\n");
        fflush(stdout);

        sem_post(&strab);

    }

}

void* Trabajador(void* args){

    while(1){

        for(int i=0;i<3;i++){

            sem_wait(&strab);

            printf("Comienza a trabajar\n");
            fflush(stdout);

            for(int i=0;i<3;i++){
                printf(" %d\n",med[i]);
                fflush(stdout);
            }
            printf("\n");
            sleep(rand()%5);

            printf("Terminamos de tarbajar\n");
            fflush(stdout);

            for(int i=0;i<3;i++){
                sem_post(&s[i]);
            }
        
        }
    }

}



int main(){

    srand(time(NULL));

    pthread_t h[4]; 

    sem_init(&s[0],0,1);
    sem_init(&s[1],0,1);
    sem_init(&s[2],0,1);
    sem_init(&strab,0,0);

    int id[3];

    for(int i=0;i<3;i++){
        id[i] = i;
        pthread_create(&h[i],NULL,Sensor,&id[i]);
    }
    pthread_create(&h[4],NULL,Trabajador,NULL);

    for(int i=0;i<4;i++){
        pthread_join(h[i],NULL);
    }

}