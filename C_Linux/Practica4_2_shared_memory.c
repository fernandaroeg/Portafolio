/* Creación de una zona de memoria compartida */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
/* Fichero que contiene la información común (clave y estructura de datos) */
#include "common.h"

/*Función auxiliar para la lógica de adivinanza*/
int diff(int min, int max, int num_intentos)         
{
    int result;
	if (num_intentos > 0)
		{
			result = (max-min)/(2*num_intentos);
			//printf("(busted)diff  es %d \n", result);
		}
	else
		{
			result = (max-min)/(2);
			//printf("(busted)diff  es %d \n", result);
			
		}
	if(result<1)
		{
			result=1;
		}
   return result;                 
}

int main(void)
{
	/*1. Declaracion de variables*/
	int shmid;
	datos *zona_comun;
	int res;
	int feedback;
	int num_intentos = 1;
	int guess=50; //primera adivinanza del Proc2 es el número medio
	int max=100;
	int min=0;
	int max_anterior, min_anterior, dif;
	
	/*2. Configuración memoría compartida*/
	shmid = shmget(CLAVE_SHM, sizeof(datos), IPC_CREAT|0666);
	if(shmid== -1)
		{
			perror("shmget");
			exit(EXIT_FAILURE);
		}	
	zona_comun = (datos*) shmat(shmid,0,0);
	printf("(Proc2)Mi adivinanza inicial es el número %d \n",guess);
	
	/*3. Lógica del programa*/
	feedback= zona_comun->feedback_Proc1;
	while(feedback!=guess)
	{
		while( zona_comun->turno==1) {/*busy waiting*/}
		/*4. Entra mi turno*/
		zona_comun->respuesta_Proc2=guess;
		feedback=zona_comun->feedback_Proc1;
		
		/* 5. Lógica del Proc2 */	
		if ( num_intentos <= 9)
			{
				/* Lógica de la adivinanza*/
				if (feedback==-4)///////////////menor
					{
					//printf("(Proc2)He recibido el feedback de Proc1 = menor \n");						
					if(num_intentos >1)
						{
							min=min_anterior;
							max=guess;
							dif= diff(min, max, num_intentos);
							guess=guess - dif;
						}
					else
						{
							max=50;
							min=0;
							dif= diff(min, max, num_intentos);
							guess=guess - dif;
						}
					zona_comun->respuesta_Proc2=guess;
					printf("(Proc2)Mi nueva propuesta es %d, intento:%d \n", guess, num_intentos+1);
					//printf("(Proc2)diff  es %d \n", dif);
					}
					
				else if (feedback==-2)/////////////mayor
					{
					//printf("(Proc2)He recibido el feedback de Proc1 = mayor \n");					
					if(num_intentos >1)
						{
							min=guess;
							max=max_anterior;
							dif= diff(min, max, num_intentos);
							guess=guess + dif;
						}
				else
					{
						max=100;
						min=50;
						dif= diff(min, max, num_intentos);
						guess=guess + dif;
					}
				zona_comun->respuesta_Proc2=guess;
				printf("(Proc2)Mi nueva propuesta es %d, intento:%d \n", guess, num_intentos+1);
				//printf("(Proc2)diff  es %d \n", dif);
				}						
			else if (feedback ==-10)//igual
				{
				//printf("(Proc2)He recibido el feedback de Proc1 = adivinaste \n");
				printf("(Proc2)Adiviné! termina el juego \n");
				exit(EXIT_SUCCESS);
				}			
			min_anterior = min;
			max_anterior = max;
			num_intentos+=1;
		}
		else if (num_intentos>=10)
		{
			printf("(Proc2)Num de intentos agotado, perdí :( \n");
			exit(EXIT_SUCCESS); //fin del programa
		} 
		zona_comun->turno=1;	// cedo el turno: 
	}
		
	shmctl(shmid, IPC_RMID,0); 	// Borrado de la zona de memoria compartida 
	if(res==-1)
		{
			perror("shmctl");
			exit(EXIT_FAILURE);
		}	
	exit(EXIT_SUCCESS);
}


	