/* A group of M soldiers and a general are surrounded by the enemy and there is no possibility of
victory without reinforcements, but there is only one horse to escape and call for said reinforcements. The
Soldiers reach an agreement to determine who will escape and ask for help. The general
choose a number N at random and one of the soldiers from it.
Starting with the soldier chosen by the general, they count clockwise,
N soldiers, discarding the soldier found in the Nth place. The check
starts again with the next soldier (the one who follows the eliminated one according to the direction of the
clockwise). The process continues so that every time the count reaches N it is discarded
to a soldier to escape. Once a soldier is removed from the circle he is no longer counted.
The soldier left at the end is the one who takes the horse and escapes. */

#include <mqueue.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define N 5 // Nº de soldados

mqd_t general_soldado[N], soldados[N];

struct envio
{ // Estructura de envio
	int salto;
	int total_descartados;
	int cuenta;
};

void *general(void *attr)
{

	int soldado_elegido = rand() % N; // Elige al soldado
	int prio = 0;
	int i; // Iterador
	struct envio e;

	// Inicializo la estructura
	e.salto = rand() % (N - 1) + 1; // Tamaño de salto entre soldados
	e.cuenta = 0;					// Contador para saber cuando se ha llegado al soldado a descartar
	e.total_descartados = 0;

	printf("Soy el general, y elijo al soldado %d con salto %d\n", soldado_elegido, e.salto);

	for (i = 0; i < N; i++)
	{
		if (i == soldado_elegido)
		{
			e.cuenta = -1; // De esta forma el elegido sabe que es él
			if (mq_send(general_soldado[i], (char *)&e, sizeof(struct envio), prio) == -1)
			{
				perror("mq_send");
				exit(1);
			}
		}
		else
		{
			e.cuenta = 0; // Valor para no elegidos
			if (mq_send(general_soldado[i], (char *)&e, sizeof(struct envio), prio) == -1)
			{
				perror("mq_send");
				exit(1);
			}
		}
	}

	pthread_exit(NULL);
}

void *soldado(void *attr)
{

	int id = *((int *)attr);
	int prio = 0;
	int estoy_descartado = 0;
	struct envio e;
	int fin = 0;

	// Comienza recibiendo la estructura del general (y designando asi al elegido)
	if (mq_receive(general_soldado[id], (char *)&e, sizeof(struct envio), &prio) == -1)
	{
		perror("mq_receive");
		exit(1);
	}
	if (e.cuenta == -1)
	{ // Elegido, comienza enviando mensajes
		printf("Soy el soldado %d y soy el elegido\n", id);
		e.cuenta = 0;
		if (mq_send(soldados[id], (char *)&e, sizeof(struct envio), prio) == -1)
		{
			perror("mq_send");
			exit(1);
		}
	}
	else
	{
		//printf("Soy el soldado %d y no he sido elegido para comenzar\n", id);
	}

	while (!fin)
	{
		if (id == 0)
		{
			if (mq_receive(soldados[N - 1], (char *)&e, sizeof(struct envio), &prio) == -1)
			{
				perror("mq_receive");
				exit(1);
			}
		}
		else
		{
			if (mq_receive(soldados[id - 1], (char *)&e, sizeof(struct envio), &prio) == -1)
			{
				perror("mq_receive");
				exit(1);
			}
		}
		printf("Soldado %d recibe\n", id);

		if (!estoy_descartado)
		{
			/* Miro si ya solo quedo yo */
			if (e.total_descartados == N - 1)
			{
				printf("Soldado %d GANA\n", id);
				e.cuenta = -1; // Valor especial para terminar
				fin = 1;
			}
			else
			{
				e.cuenta++;
				if (e.cuenta == e.salto)
				{
					// me descarto
					estoy_descartado = 1;
					e.cuenta = 0;
					e.total_descartados++;
					printf("Soldado %d descartado. Total descartados: %d\n", id, e.total_descartados);
				}
			}
		}
		else
		{
			fin = e.cuenta == -1;
		}

		printf("Soldado %d transmite. Valor cuenta: %d\n", id, e.cuenta);
		// Transmito estado actual
		if (mq_send(soldados[id], (char *)&e, sizeof(struct envio), prio) == -1)
		{
			perror("mq_send");
			exit(1);
		}
	}

	/* Si he ganado, me espero a recibir el mensaje de terminacion que esta circulando */
	/* Esto no es necesario, pero es para no dejar mensajes en las colas */
	if (!estoy_descartado)
	{
		if (id == 0)
		{
			if (mq_receive(soldados[N - 1], (char *)&e, sizeof(struct envio), &prio) == -1)
			{
				perror("mq_receive");
				exit(1);
			}
		}
		else
		{
			if (mq_receive(soldados[id - 1], (char *)&e, sizeof(struct envio), &prio) == -1)
			{
				perror("mq_receive");
				exit(1);
			}
		}
		printf("Soldado %d recibe\n", id);
	}

	printf("Soldado %d termina\n", id);
	pthread_exit(NULL);
}

int main()
{

	srand(time(NULL));
	pthread_t th[N], thg;
	int i;
	int id[N];
	struct mq_attr attr;

	// Atributos cola mensajes
	attr.mq_maxmsg = 1;
	attr.mq_msgsize = sizeof(struct envio);
	attr.mq_flags = 0;

	char cad_general[256], cad_soldado[256];
	for (i = 0; i < N; i++)
	{
		sprintf(cad_general, "/general_soldado_%d", i);
		sprintf(cad_soldado, "/soldados_%d", i);
		mq_unlink(cad_general);
		mq_unlink(cad_soldado);

		if ((general_soldado[i] = mq_open(cad_general, O_RDWR | O_CREAT,
										  (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH), &attr)) == -1)
		{
			perror("Error creando cola");
		}
		if ((soldados[i] = mq_open(cad_soldado, O_RDWR | O_CREAT,
								   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH), &attr)) == -1)
		{
			perror("Error creando cola");
		}
	}

	// falta control de errores
	for (i = 0; i < N; i++)
	{
		id[i] = i;
		pthread_create(&th[i], NULL, soldado, &id[i]);
	}
	pthread_create(&thg, NULL, general, NULL);
	for (i = 0; i < N; i++)
	{
		pthread_join(th[i], NULL);
	}

	for (i = 0; i < N; i++)
	{
		mq_close(general_soldado[i]);
		mq_close(soldados[i]);
	}

	return 0;
}
