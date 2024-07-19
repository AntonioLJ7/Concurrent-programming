/* There are N threads connected in a network in a linear way (each thread is connected only
with its predecessor and its successor in the network, except the first and last ones that do not have a predecessor
and successor respectively). Each thread initially generates a random number and must
find out by exchanging messages what is the minimum and maximum of the numbers of all
the strands. Once the numbers have been determined, each thread will display them on the screen and terminate */

#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define N 5
#define RANGO_MAX 20

struct numeros
{
	int max;
	int min;
};

mqd_t der[N - 1]; /* Colas de mensajes hacia la derecha */
mqd_t izq[N - 1]; /* Colas de mensajes hacia la izquierda */

int encuentra_min(int num1, int num2)
{
	if (num1 < num2)
	{
		return num1;
	}
	return num2;
}

int encuentra_max(int num1, int num2)
{
	if (num1 > num2)
	{
		return num1;
	}
	return num2;
}

void *nodo(void *argg)
{
	int id = *((int *)argg);
	unsigned int s_prio, r_prio;
	struct numeros mis_num, otros_num;

	s_prio = 0;

	mis_num.min = rand() % RANGO_MAX;
	mis_num.max = mis_num.min;

	printf("[%d] Maximo: %d\n", id, mis_num.max);
	printf("[%d] Minimo: %d\n", id, mis_num.min);

	if (id != 0)
	{
		if ((mq_receive(der[id - 1], (char *)&otros_num, sizeof(struct numeros), &r_prio)) == -1)
		{
			perror("Error recibiendo numeros [1]\n");
			exit(1);
		}

		mis_num.min = encuentra_min(mis_num.min, otros_num.min);
		mis_num.max = encuentra_max(mis_num.max, otros_num.max);
	}

	if (id != (N - 1))
	{
		if ((mq_send(der[id], (const char *)&mis_num, sizeof(struct numeros), s_prio)) == -1)
		{
			perror("Error enviando numeros [1]\n");
			exit(1);
		}

		if ((mq_receive(izq[id], (char *)&otros_num, sizeof(struct numeros), &r_prio)) == -1)
		{
			perror("Error recibiendo numeros [2]\n");
			exit(1);
		}

		mis_num.min = otros_num.min;
		mis_num.max = otros_num.max;
	}

	if (id != 0)
	{
		if ((mq_send(izq[id - 1], (const char *)&mis_num, sizeof(struct numeros), s_prio)) == -1)
		{
			perror("Error enviando numeros [2]\n");
			exit(1);
		}
	}

	printf("[%d] Maximo total: %d\n", id, mis_num.max);
	printf("[%d] Minimo total: %d\n", id, mis_num.min);

	pthread_exit(NULL);
}

int main()
{
	pthread_t nodos[N];
	int id[N];
	struct mq_attr attr;

	srand(time(NULL));

	/*Atributos de la cola de mensajes*/
	attr.mq_maxmsg = 1;
	attr.mq_msgsize = sizeof(struct numeros);
	attr.mq_flags = 0;

	char cad[255];

	/*Creacion de colas de mensajes*/
	for (int i = 0; i < (N - 1); i++)
	{
		sprintf(cad, "/der%d", i);

		if ((mq_unlink(cad)) == -1)
		{
			//			perror("Error [1]\n");
		}

		der[i] = mq_open(cad, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, &attr);

		if (der[i] == -1)
		{
			perror("Error creando cola de mensajes\n");
		}

		sprintf(cad, "/izq%d", i);

		if ((mq_unlink(cad)) == -1)
		{
			//			perror("Error [2]\n");
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
