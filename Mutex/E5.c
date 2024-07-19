/* Suppose we are in a nightclub and it turns out that the girls' service is broken.
and everyone has to share the boys'. The aim is to establish an entry protocol to the
service using mutex in which the following conditions are met:
• girls there can only be one at a time in the service.
• boys there can be more than one, with a maximum of five 

Girls have preference over boys. This means that if a boy is
waiting and a girl arrives, she must pass first. Even if she can't get into the service
because there is a girl, she will pass before the boy when the girl comes out */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_CHICOS 10
#define NUM_CHICAS 2
#define MAX_DELAY 5
#define LIMITE_CHICOS 5
#define CHICO 0
#define CHICA 1

pthread_mutex_t wc = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t espera_chico = PTHREAD_COND_INITIALIZER;
pthread_cond_t espera_chica = PTHREAD_COND_INITIALIZER;

int personas_dentro[2] = {0, 0};
int chicas_esperando = 0;
char *sexos[2] = {"el chico", "la chica"};

void *hebra_chico(void *argg)
{
	int id = *(int *)argg;

	while (1)
	{
		pthread_mutex_lock(&wc);

		while (personas_dentro[CHICA] || chicas_esperando || (personas_dentro[CHICO] >= LIMITE_CHICOS))
			pthread_cond_wait(&espera_chico, &wc);

		personas_dentro[CHICO]++;
		printf("Soy %s %d entrando (chicos: %d) (chicas %d)\n", sexos[CHICO], id, personas_dentro[CHICO], personas_dentro[CHICA]);
		pthread_mutex_unlock(&wc);

		sleep(rand() % MAX_DELAY);

		pthread_mutex_lock(&wc);
		personas_dentro[CHICO]--;
		printf("Soy %s %d saliendo (chicos: %d) (chicas %d)\n", sexos[CHICO], id, personas_dentro[CHICO], personas_dentro[CHICA]);
		if (chicas_esperando && (!personas_dentro[CHICO]))
			pthread_cond_signal(&espera_chica);
		else
			pthread_cond_signal(&espera_chico);

		pthread_mutex_unlock(&wc);
	}
}

void *hebra_chica(void *argg)
{
	int id = *(int *)argg;

	while (1)
	{
		pthread_mutex_lock(&wc);

		while (personas_dentro[CHICO] || personas_dentro[CHICA])
		{
			chicas_esperando++;
			pthread_cond_wait(&espera_chica, &wc);
			chicas_esperando--;
		}

		personas_dentro[CHICA]++;
		printf("Soy %s %d entrando (chicos: %d) (chicas %d)\n", sexos[CHICA], id, personas_dentro[CHICO], personas_dentro[CHICA]);
		pthread_mutex_unlock(&wc);

		sleep(rand() % MAX_DELAY);

		pthread_mutex_lock(&wc);
		personas_dentro[CHICA]--;
		printf("Soy %s %d saliendo (chicos: %d) (chicas %d)\n", sexos[CHICA], id, personas_dentro[CHICO], personas_dentro[CHICA]);
		if (chicas_esperando)
			pthread_cond_signal(&espera_chica);
		else
			pthread_cond_broadcast(&espera_chico);

		pthread_mutex_unlock(&wc);
		sleep(rand() % MAX_DELAY);
	}
}

int main()
{
	pthread_t chico[NUM_CHICOS], chica[NUM_CHICAS];
	int ids[NUM_CHICOS], ids2[NUM_CHICAS], i;

	srand(time(NULL));

	// falta controlar errores.
	for (i = 0; i < NUM_CHICOS; i++)
	{
		ids[i] = i;
		pthread_create(&chico[i], NULL, hebra_chico, &ids[i]);
	}

	for (i = 0; i < NUM_CHICAS; i++)
	{
		ids2[i] = i;
		pthread_create(&chico[i], NULL, hebra_chica, &ids2[i]);
	}

	pthread_exit(NULL);
}
