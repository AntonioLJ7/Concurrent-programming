/* In the “narrow bridge” problem we want to simulate the passage of cars over a bridge
narrow so that cars can only circulate in one direction at a time.
Implement this problem in C and with semaphores with the following characteristics:
– There are N cars (each car representing a thread) trying to move from
left to right or right to left.
– At any time the bridge can be either empty or occupied by passing cars.
only in one direction (left-right or right-left). To simplify,
It can be assumed that an indefinite number of vehicles can circulate at the same time.
cars in the same direction.
– Cars periodically carry out the same process: wait until they can
enter the bridge, cross the bridge and leave the bridge and, to simplify, always in the
same sense. After leaving the bridge, they wait a random amount of time until they return
to try to enter. It is also not necessary to control that the cars that enter before,
leave the bridge before others who arrive later. */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_COCHES 10
#define MAX_DELAY 2
#define IZQDER 0
#define DERIZQ 1

sem_t sentido[2], puente;
int numCoches[2] = {0,0};
char* sentidoS[2]={"izquierda", "derecha"};

void *Coche(void* arg) {
	int id = *(int*)arg;
	int miSentido = rand()%2;

	while (1) {
		
		sem_wait(&sentido[miSentido]);
	
		if (numCoches[miSentido] == 0) {
			sem_wait(&puente);
		}
	
		numCoches[miSentido]++;
		printf("Coche %d: entrando al puente por la %s (%d)\n", id, sentidoS[miSentido], numCoches[miSentido]);

		sem_post(&sentido[miSentido]);
		
	
		sleep(rand()%MAX_DELAY);
		
		
		sem_wait(&sentido[miSentido]);
	
		numCoches[miSentido]--;
		printf("Coche %d saliendo del puente (%d)\n", id, numCoches[miSentido]);
	
		if (numCoches[miSentido] == 0) {
			printf("El puente está vacío\n");
			sem_post(&puente);
		}
		sem_post(&sentido[miSentido]);
	}
}

int main() {
	int i, ids[NUM_COCHES];
	pthread_t coches[NUM_COCHES];
	pthread_attr_t attr;

	srand(time(NULL));

	sem_init(&puente, 0, 1);

	
	sem_init(&sentido[IZQDER], 0, 1);
	sem_init(&sentido[DERIZQ], 0, 1);

	pthread_attr_init(&attr);

	for (i = 0; i < NUM_COCHES; i++) {
		ids[i] = i;
		pthread_create(&coches[i],&attr,Coche,&ids[i]);
	}

	pthread_join(coches[0], NULL);
}
