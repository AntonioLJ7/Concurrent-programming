/* Using message passing, design a program to sort N numbers with a pipeline of
N threads and a generating thread of the N numbers. The generator passes through all of them one by one.
the numbers to the first strand of the pipeline. Each thread i of the pipeline successively receives
numbers of strand i – 1 and always keeps the smallest of them, passing the rest to the strand
i + 1. Since each thread knows how many numbers are going to arrive, it will also know when it is not going to
receive more numbers. Wait for that moment so that each thread shows its number perscreen and finish. */

#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define N 5
#define RANGO_MAX 100

mqd_t cola[N]; /*Colas de mensajes hacia la derecha*/

void *generadora(void *argg)
{
	int num;

	unsigned int s_prio;
	s_prio = 0;

	for (int i = 0; i < N; i++)
	{
		num = rand() % RANGO_MAX;
		//printf("Numero generado: %d\n", num);
		if ((mq_send(cola[0], (const char *)&num, sizeof(int), s_prio)) == -1)
		{
			perror("Error enviando numero [1]\n");
			exit(1);
		}
	}

	pthread_exit(NULL);
}

void ordenadora(void *argg)
{
	int id = *((int *)argg);
	int num_ant, num_act;

	unsigned int r_prio, s_prio;

	s_prio = 0;

	for (int i = 0; i < (N - id); i++)
	{
		if ((mq_receive(cola[id], (char *)&num_act, sizeof(int), &r_prio)) == -1)
		{
			perror("Error recibiendo numero [1]\n");
			exit(1);
		}

		if (i == 0)
		{
			num_ant = num_act; // Mi numero
			if (id == (N - 1))
			{
				printf("[%d]:%d\n", id, num_ant); // Hemos terminado
			}
		}
		else
		{
			if (num_act < num_ant)
			{ // Me quedo con el nuevo
				if (i == (N - id - 1))
				{
					printf("[%d]:%d\n", id, num_act);
				}

				// En cualquier caso, reenvio el mayor (el mio)
				if ((mq_send(cola[id + 1], (const char *)&num_ant, sizeof(int), s_prio)) == -1)
				{
					perror("Error enviando numero [2]\n");
					exit(1);
				}
				num_ant = num_act;
			}
			else
			{ // Me quedo con el mio
				if (i == (N - id - 1))
				{
					printf("[%d]:%d\n", id, num_ant);
				}

				// En cualquier caso, reenvio el mayor (el nuevo)

				if ((mq_send(cola[id + 1], (const char *)&num_act, sizeof(int), s_prio)) == -1)
				{
					perror("Error enviando numero [2]\n");
					exit(1);
				}
			}
		}
	}
	pthread_exit(NULL);
}

int main()
{
	pthread_t generador, hebras[N];
	int id[N];

	struct mq_attr attr;

	srand(time(NULL));

	/*Atributos de la cola de mensajes*/
	attr.mq_maxmsg = 1;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags = 0;

	char cad[256];

	/*Creacion de colas de mensajes*/
	for (int i = 0; i < N; i++)
	{
		sprintf(cad, "/cola%d", i);

		if ((mq_unlink(cad)) == -1)
		{
			//perror("Error\n");
		}

		cola[i] = mq_open(cad, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, &attr);

		if (cola[i] == -1)
		{
			perror("Error creando cola de mensajes\n");
			exit(1);
		}
	}

	/*Creacion de las hebras*/
	if ((pthread_create(&generador, NULL, generadora, NULL)) != 0)
	{
		perror("Error creando hebra generadora\n");
		exit(1);
	}

	for (int i = 0; i < N; i++)
	{
		id[i] = i;
		if ((pthread_create(&hebras[i], NULL, (void *)ordenadora, &id[i])) != 0)
		{
			perror("Error creando hebra ordenadora\n");
			exit(1);
		}
	}

	/*Esperamos a que las hebras terminen*/
	pthread_join(generador, NULL);
	for (int i = 0; i < N; i++)
	{
		pthread_join(hebras[i], NULL);
	}

	// falta cerrar colas
	return 0;
}
