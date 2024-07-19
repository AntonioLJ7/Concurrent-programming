/* We want to carry out a simplified simulation of the air traffic of an airport taking into account
It has the following characteristics:
– There is a single landing strip shared by all planes.
– Planes can approach from the north or south, therefore, to control the
air traffic there is a controlling strand for the north and another for the south.
– There are N planes represented, each with a thread that periodically tries
Land at the airport (whichever side you want). The landing procedure
is the following:
• The airplane “requests permission” to land from the controller on the side on which it is flying.
approaches and waits (using a traffic light) for the
permission.
• The controlling processes attend to landing requests (one at a time).
When they get a request, they have to wait until the track is ready.
available and then they notify the plane.
• When an airplane is cleared to land, the landing will be simulated with a
random time during which the track is busy.
Messages should be displayed indicating when a plane begins to land and when
landing ends. */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10

sem_t control_Sur,control_Norte,hay_aviones_N,hay_aviones_S,tengo_pista_S,
tengo_pista_N,espera_aterrizaje_N,espera_aterrizaje_S,pista;


void* AvionS(){

    while(1){

        sem_wait(&control_Sur); //Esperamos a control
        sem_post(&hay_aviones_S); //La line aerea del Norte esta ocuapda

        sem_wait(&tengo_pista_S); //Esperamos a que la psita este libre
        printf("Avion procedente del Sur aterrizando\n");
        fflush(stdout);
        sleep(5);

        sem_post(&espera_aterrizaje_S); // La pista esta libre
        sem_post(&control_Sur); //Avisamos a control
    }

}

void* fControladorS(){

    while(1){
        sem_wait(&hay_aviones_S);
        sem_wait(&pista);

        printf("Pista reservada al controlador del Sur\n");
        fflush(stdout);

        sem_post(&tengo_pista_S);
        sem_wait(&espera_aterrizaje_S);

        printf("Avion del Sur esta aterrizando\n");
        sleep(5);

        sem_post(&pista);
    }

}

void* AvionN(){

    while(1){

        sem_wait(&control_Norte);
        sem_post(&hay_aviones_N);

        sem_wait(&tengo_pista_N);
        printf("Avion procedente del Norte aterrizando\n");
        fflush(stdout);
        sleep(5);

        sem_post(&espera_aterrizaje_N);
        sem_post(&control_Norte);

    }

}

void* fControladorN(){

    while(1){

        sem_wait(&espera_aterrizaje_N);
        sem_wait(&pista);

        printf("Pista revervada al controlador Note\n");
        fflush(stdout);

        sem_post(&tengo_pista_N);
        sem_wait(&espera_aterrizaje_N);

        printf("Avion procedente el Norte ha completado su aterrizaje\n");
        sleep(5);

        sem_post(&pista);

    }

}

int main(){
    pthread_t aviones_N[5];
    pthread_t aviones_S[5];
    pthread_t controlN,controlS;

    sem_init(&control_Norte,0,1);
    sem_init(&control_Sur,0,1);
    sem_init(&hay_aviones_N,0,0);
    sem_init(&hay_aviones_S,0,0);
    sem_init(&tengo_pista_N,0,0);
    sem_init(&tengo_pista_S,0,0);
    sem_init(&espera_aterrizaje_S,0,0);
    sem_init(&espera_aterrizaje_N,0,0);
    sem_init(&pista,0,1);

    for(int i=0;i<5;i++){
        pthread_create(&aviones_N[i],NULL,AvionN,NULL);
        pthread_create(&aviones_S[i],NULL,AvionS,NULL);
    }
    pthread_create(&controlN,NULL,fControladorN,NULL);
    pthread_create(&controlS,NULL,fControladorS,NULL);


    for(int i=0;i<3;i++){
        pthread_join(aviones_N[i],NULL);
        pthread_join(aviones_S[i],NULL);
    }
    pthread_join(controlN,NULL);
    pthread_join(controlS,NULL);
}