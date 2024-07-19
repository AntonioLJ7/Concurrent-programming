/* Implement a program in C that simulates the operation of a smoking room in the
that people want to enter. The room has a maximum capacity M and there are N>M people
that they will try to enter it. Model the system with one thread for each person
will behave in the following way:
• Each person will try to enter the room if they are not inside.
• If he enters the room he will smoke a cigarette for a while and leave it.
• When you leave, you will try to enter again after a certain time */

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_FUMADORES 5
#define NUM_MAXPERSONAS 10
#define TFUMAR 5

pthread_mutex_t sala = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t intento_entrar = PTHREAD_COND_INITIALIZER;
int t_sala = 0;

void *hebra_fumador(void *argg){

    int id = *(int*)argg;

    while(1){

        pthread_mutex_lock(&sala);

        while(t_sala > NUM_MAXPERSONAS){
            pthread_cond_wait(&intento_entrar,&sala);
        }
        t_sala++;

        printf("La persona %d entra en la sala\n",id+1);
        pthread_mutex_unlock(&sala);

        printf("Se pone a fumar\n");
        sleep(rand()%TFUMAR);

        pthread_mutex_lock(&sala);
         t_sala--;
         printf("La persona %d sale de la sala\n",id+1);

        pthread_mutex_unlock(&sala);

        pthread_cond_signal(&intento_entrar);
        sleep(rand()%3*TFUMAR);
    }

}

int main(){
    
    pthread_t fumador[NUM_FUMADORES];
    pthread_attr_t attr;
    int id[NUM_FUMADORES];

    pthread_attr_init (&attr);

    srand(time(NULL));
    
    for(int i=0;i<NUM_FUMADORES;i++){
        id[i]=i;
        pthread_create(&fumador[i],&attr,hebra_fumador,&id[i]);
    }

    pthread_exit(NULL);
}
