/*  En una red de N hebras, cada hebra genera inicialmente un número aleatorio. Posteriormente, 
la primera hebra de la red debe averiguar cuáles de las otras hebras tienen también su número 
aleatorio, mostrando el identificador de éstas por pantalla. Como restricción, la red a construir 
debe ser una red de tipo lineal, donde cada hebra se comunica únicamente con otras 2 hebras 
(anterior y posterior), excepto la primera y última hebra de la red que sólo están conectadas 
con una hebra cada una. */

#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define N 5
#define RANGO_MAX 5

mqd_t izq[N - 1];

void *nodo(void *argg)
{
	int id = *((int *)argg);
	unsigned int s_prio, r_prio;
	int num;
	int numeros[N];

	num = rand() % RANGO_MAX;
	printf("[%d]: %d\n", id, num);

	if (id == (N - 1))
	{ /*Si es el ultimo nodo*/

		/*Inicializacion array numeros*/
		for (int i = 0; i < N - 1; i++)
		{
			numeros[i] = -1;
		}
		numeros[id] = num;

		s_prio = 0;

		if ((mq_send(izq[id], (const char *)&numeros, sizeof(numeros), s_prio)) == -1)
		{
			perror("Error enviando numeros [1]\n");
			exit(1);
		}
	}
	else
	{
		if ((mq_receive(izq[id + 1], (char *)&numeros, sizeof(numeros), &r_prio)) == -1)
		{
			perror("Error recibiendo numeros [2]\n");
			exit(1);
		}

		numeros[id] = num;

		if (id != 0)
		{
			if ((mq_send(izq[id], (const char *)&numeros, sizeof(numeros), s_prio)) == -1)
			{
				perror("Error enviando numeros [2]\n");
				exit(1);
			}
		}
		else
		{ /*Si es la primera hebra*/
			for (int i = 1; i < N; i++)
			{
				if (numeros[i] == numeros[0])
				{
					printf("[%d] ha coincidido con la primera hebra\n", i);
				}
			}
		}
	}

	pthread_exit(NULL);
}

int main()
{
	pthread_t nodos[N];
	int id[N], numeros[N];
	struct mq_attr attr;

	srand(time(NULL));

	/*Atributos de la cola de mensajes*/
	attr.mq_maxmsg = 1;
	attr.mq_msgsize = sizeof(numeros);
	attr.mq_flags = 0;

	char cad[255];

	/*Creacion de colas de mensajes*/
	for (int i = 1; i < N; i++)
	{
		sprintf(cad, "/izq%d", i);

		if ((mq_unlink(cad)) == -1)
		{
			//			perror("Error\n");
		}

		izq[i] = mq_open(cad, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, &attr);

		if (izq[i] == -1)
		{
			perror("Error creando cola de mensajes\n");
			exit(1);
		}
	}

	for (int i = 0; i < N; i++)
	{
		id[i] = i;
		if ((pthread_create(&nodos[i], NULL, nodo, &id[i])) != 0)
		{
			perror("Error creando hebra\n");
			exit(1);
		}
	}

	pthread_exit(NULL);
	// falta cerrar colas

	return 0;
}
