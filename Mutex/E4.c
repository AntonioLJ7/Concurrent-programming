/* A tribe of savages communally dine on a large pot containing M missionaries.
cooked. When a savage wants to eat, he helps himself to a missionary from the pot, unless
that is empty. If the pot is empty, the savage wakes up the cook and waits for him to fill the pot.
cooking pot. Develop the code for the actions of the cook and the wild ones using mutex */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_MISIONEROS 5
#define NUM_SALVAJES 10
#define MAX_DELAY 5

pthread_mutex_t olla = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t espera_olla_vacia = PTHREAD_COND_INITIALIZER;
pthread_cond_t espera_olla_llena = PTHREAD_COND_INITIALIZER;
int misioneros = 0;

void *hebra_cocinero(void *argg)
{
	while (1)
	{
		pthread_mutex_lock(&olla);
		while (misioneros)
			pthread_cond_wait(&espera_olla_vacia, &olla);

		printf("Soy el cocinero y voy a cocinar a %d misioneros\n", NUM_MISIONEROS);

		sleep(rand() % MAX_DELAY);
		misioneros = NUM_MISIONEROS;

		pthread_cond_broadcast(&espera_olla_llena);
		pthread_mutex_unlock(&olla);
	}
}

void *hebra_salvaje(void *argg)
{
	int id = *(int *)argg;

	while (1)
	{
		pthread_mutex_lock(&olla);
		while (!misioneros)
		{
			pthread_cond_signal(&espera_olla_vacia);
			pthread_cond_wait(&espera_olla_llena, &olla);
		}

		printf("Soy el salvaje %d y voy a comer, quedan %d misioneros en la olla\n", id, --misioneros);
		pthread_mutex_unlock(&olla);

		sleep(rand() % MAX_DELAY); // COMIENDO
	}
}

int main()
{
	pthread_t salvaje[NUM_SALVAJES], chef;
	int i, ids[NUM_SALVAJES];

	srand(time(NULL));

	// falta control de errores
	pthread_create(&chef, NULL, hebra_cocinero, NULL);

	for (i = 0; i < NUM_SALVAJES; i++)
	{
		ids[i] = i;
		pthread_create(&salvaje[i], NULL, hebra_salvaje, &ids[i]);
	}

	pthread_exit(NULL);
}
