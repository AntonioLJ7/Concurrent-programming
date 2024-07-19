/* A pasture is being used by cows and sheep. In the pasture there is room for both M
animals of the same type. Model the grassland problem with mutex taking into account the
following features:
• There are N>M animals trying to enter the grassland. Each animal will enter the
grassland only if it is empty or if there are animals of the same type within it.
• The pasture may be empty or with animals of only one type within it.
• Each animal performs the same process several times. Try to enter the grassland,
It is grazing for a while and comes out of it. Once he leaves the pasture he will try
re-enter after a certain amount of time */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_PASTIZAL 5
#define NUM_ANIMALES 10
#define MAX_DELAY 5

pthread_mutex_t pastizal = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t espera_vaca = PTHREAD_COND_INITIALIZER;
pthread_cond_t espera_oveja = PTHREAD_COND_INITIALIZER;

int vacas_dentro = 0;
int ovejas_dentro = 0;

void *hebra_vaca(void *argg)
{
	int id = *(int *)argg;

	while (1)
	{
		pthread_mutex_lock(&pastizal);
		while (ovejas_dentro)
		{
			pthread_cond_wait(&espera_vaca, &pastizal);
		}

		vacas_dentro++;
		printf("La vaca %d entra en el pastizal (%d v %d o dentro)\n", id, vacas_dentro, ovejas_dentro);

		pthread_mutex_unlock(&pastizal);

		sleep(rand() % MAX_DELAY);

		pthread_mutex_lock(&pastizal);
		vacas_dentro--;
		printf("La vaca %d sale del pastizal (%d v %d o dentro)\n", id, vacas_dentro, ovejas_dentro);
		pthread_cond_signal(&espera_oveja);

		pthread_mutex_unlock(&pastizal);
		sleep(rand() % 3*MAX_DELAY);
	}
}

void *hebra_oveja(void *argg)
{
	int id = *(int *)argg;

	while (1)
	{
		pthread_mutex_lock(&pastizal);
		while (vacas_dentro)
		{
			pthread_cond_wait(&espera_oveja, &pastizal);
		}

		ovejas_dentro++;
		printf("La oveja %d entra en el pastizal (%d v %d o dentro)\n", id, vacas_dentro, ovejas_dentro);

		pthread_mutex_unlock(&pastizal);

		sleep(rand() % MAX_DELAY);

		pthread_mutex_lock(&pastizal);
		ovejas_dentro--;
		printf("La oveja %d sale del pastizal (%d v %d o dentro)\n", id, vacas_dentro, ovejas_dentro);
		pthread_cond_signal(&espera_vaca);

		pthread_mutex_unlock(&pastizal);
		sleep(rand() % 3*MAX_DELAY);
	}
}

int main()
{
	pthread_t vaca[NUM_ANIMALES], oveja[NUM_ANIMALES];
	pthread_attr_t attr;
	int i, id[NUM_ANIMALES];

	srand(time(NULL));

	// falta controlar errores. Compartimos ids entre vacas y ovejas
	for (i = 0; i < NUM_ANIMALES; i++)
	{
		id[i] = i;
		pthread_create(&vaca[i], NULL, hebra_vaca, &id[i]);
		pthread_create(&vaca[i], NULL, hebra_oveja, &id[i]);
	}

	pthread_exit(NULL);
}
