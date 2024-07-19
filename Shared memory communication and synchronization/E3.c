/* We want to simulate the behavior of the water level in a lake. The lake receives water from
two rivers and can lower their level through the opening of two dams. Model using
Wait activates the proposed system taking into account the following aspects:
– Model the system with one strand for each river, one strand for each dam and one
global variable of type integer to represent the water level of the lake.
– You want to have exact control of the water level. Therefore, the variations in the
level (increments or decrements) must be performed in mutual exclusion.
– Additional threads cannot be used to achieve mutual exclusion.
– Each river will increase the water level 1000 times (1 unit at each access to the
mutual exclusion zone).
– Each dam will decrease the water level 1000 times (1 unit at each access to the
mutual exclusion zone).
– If the water level is 0, the dams cannot reduce the water level.
– Every time a process increases or decreases the water level, it must be displayed
an informative message indicating the action taken and the current water level.
– If a dam attempts to lower the level when the lake is empty, a
indicative message on screen.
– The water level must be shown at the end of the program (which must be 0 after the
1000+1000 increments and 1000+1000 decrements) */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define RIO 0
#define PRESA 1
#define NHEBRA 2
#define VECES 1000

unsigned fRios[2] = {0,0};
unsigned fPresas[2] = {0,0};
unsigned fPresa_Rio[2] = {0,0};

int nivelLago = 0;
int turnoRio = 0;
int turnoPresa = 0;
int turnoRIoPresa = 0;

void *Rio(void* rios){

    int miId = *(int*)rios;
    int otro = (miId+1)%2;

    for(int i=0;i<VECES;i++){

        fRios[miId] = 1;
        turnoRio = otro;
        while(fRios[otro] && turnoRio == otro);

        fPresa_Rio[RIO] = 1;
        turnoRIoPresa = PRESA;
        while(fPresa_Rio[PRESA] && turnoRIoPresa == PRESA);
        
        nivelLago++;
        printf("[INC %d] El nivel del lago a aumentado %d\n",miId,nivelLago);

        fPresa_Rio[RIO] = 0;
        fRios[miId] = 0;

    }

    pthread_exit(NULL);
}

void *Presa(void* presas){
    int i=0;
    int miId = *(int*)presas;
    int otro = (miId+1)%2;

    while(i<VECES){

        fPresas[miId] = 1;
        turnoPresa = otro;
        while(fPresas[otro] && turnoPresa == otro);

        fPresa_Rio[PRESA] = 1;
        turnoRIoPresa = RIO;
        while(fPresa_Rio[RIO] && turnoRIoPresa == RIO);

        if(nivelLago > 0){
            nivelLago--;
            i++;
            printf("[DEC %d] El nivel decrementa %d\n",miId,nivelLago);
        }else{
            printf("No se puede decementar el nivel del lago\n");
        }

        fPresa_Rio[PRESA] = 0;
        fPresas[miId] = 0;

    }
    pthread_exit(NULL);
}

int main(){
    pthread_t rios[NHEBRA],presas[NHEBRA];
    int ids[NHEBRA];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    for(int i=0;i<NHEBRA;i++){
        ids[i] = i;
        pthread_create(&rios[i],&attr,Rio,&ids[i]);
        pthread_create(&presas[i],&attr,Presa,&ids[i]);
    }

    for(int i=0;i<NHEBRA;i++){
        pthread_join(rios[i],NULL);
        pthread_join(presas[i],NULL);
    }

    printf("El nivel del lago es %d/n",nivelLago);
}