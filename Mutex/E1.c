/* Consider a system made up of three smoking strands who spend the day rolling cigarettes.
and smoking. To roll a cigarette you need three ingredients: tobacco, paper and matches. Each
smoker has a sufficient supply (for the rest of his life) of one of the three ingredients.
Each smoker has a different ingredient, that is, there will be a smoker who has a different amount
infinite of tobacco, another of paper and the other of matches. There is also an agent thread that puts
two of the three ingredients on a table. The agent has infinite reserves of each
one of the three ingredients and randomly choose which ingredients to put
on the table. When you have put them in, the smoker who has the other ingredient can smoke
(the other two don't). To do this, he takes the ingredients, rolls a cigarette and smokes it. When it ends
smoking the cycle repeats itself. In summary, the cycle that must be repeated is:
agent puts ingredients → smoker rolls cigarette → smoker smokes → smoker
finishes smoking → agent puts ingredients → …
That is, at any given moment there is at most one smoker smoking a cigarette. */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_FUMADORES 3
#define NUM_INGREDIENTES NUM_FUMADORES
#define TFUMAR 5

char *nombre_ingredientes[NUM_INGREDIENTES] = {"tabaco", "papel", "cerillas"};

pthread_mutex_t mesa = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t puedo_fumar[NUM_FUMADORES] = {PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER};
pthread_cond_t puedo_poner_ingredientes = PTHREAD_COND_INITIALIZER;

int ingrediente_que_falta = -1, fumando = 0;

void quiero_fumar(int ingrediente)
{
	// Podré fumar cuando el ingrediente que falta en la mesa
	// sea el que yo tengo
	pthread_mutex_lock(&mesa);
	while (ingrediente_que_falta != ingrediente)
	{
		pthread_cond_wait(&puedo_fumar[ingrediente], &mesa);
	}
	ingrediente_que_falta = -1;
	fumando = 1;
	pthread_mutex_unlock(&mesa);
}

void fumar(int ingrediente)
{
	printf("Soy el fumador que tiene %s y empiezo a fumar\n", nombre_ingredientes[ingrediente]);
	sleep(rand() % TFUMAR);
}

void dejo_de_fumar(int ingrediente)
{
	pthread_mutex_lock(&mesa);
	fumando = 0;
	pthread_mutex_unlock(&mesa);
	pthread_cond_signal(&puedo_poner_ingredientes);
}

// Modelamos el hecho de que deje ingredientes indicando el que no deja
void dejo_ingredientes(int ingrediente_que_no_dejo)
{
	// Podré dejar un ingrediente cuando la mesa esté vacía y
	// ningún fumador esté fumando
	pthread_mutex_lock(&mesa);

	// es necesario esperar por ambos motivos para no "machacar" los ingredientes ya puestos
	// si no viene el fumador, y para esperar a que se fume
	while (ingrediente_que_falta >= 0 || fumando)
	{
		pthread_cond_wait(&puedo_poner_ingredientes, &mesa);
	}
	ingrediente_que_falta = ingrediente_que_no_dejo;
	printf("El agente deja sobre la mesa %s y %s\n", nombre_ingredientes[(ingrediente_que_no_dejo + 1) % 3], nombre_ingredientes[(ingrediente_que_no_dejo + 2) % 3]);
	pthread_mutex_unlock(&mesa);
	pthread_cond_signal(&puedo_fumar[ingrediente_que_no_dejo]);
}

void *hebra_fumador(void *argg)
{
	int ingrediente = *(int *)argg;

	while (1)
	{
		quiero_fumar(ingrediente);
		fumar(ingrediente);
		dejo_de_fumar(ingrediente);
	}
}

void *hebra_agente(void *argg)
{
	int ingrediente_que_no_dejo;

	while (1)
	{
		ingrediente_que_no_dejo = rand() % NUM_INGREDIENTES;
		dejo_ingredientes(ingrediente_que_no_dejo);
	}
}

int main()
{
	pthread_t fumadores[NUM_FUMADORES], agente;
	pthread_attr_t attr;
	int tabaco = 0, papel = 1, cerillas = 2;

	srand(time(NULL));
	pthread_attr_init(&attr);

	// Falta control de errores
	pthread_create(&fumadores[tabaco], &attr, hebra_fumador, &tabaco);
	pthread_create(&fumadores[papel], &attr, hebra_fumador, &papel);
	pthread_create(&fumadores[cerillas], &attr, hebra_fumador, &cerillas);
	pthread_create(&agente, &attr, hebra_agente, NULL);

	pthread_exit(NULL);
}
