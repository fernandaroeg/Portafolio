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
/* Fichero que contiene la información común (clave y estructura’ de datos) */
#include "common.h"

int main(void)
{
	/*1. Declaracion de variables*/
	int shmid;
	datos *zona_comun;
	int rand, intento, res;
	int num_intentos = 1;
	
	/*2. Configuración memoría compartida*/
	shmid = shmget(CLAVE_SHM, sizeof(datos), IPC_CREAT|0666);
	if(shmid== -1)
		{
			perror("shmget");
			exit(EXIT_FAILURE);
		}
	zona_comun = (datos*) shmat(shmid,0,0);
	
	/*3. Inicialización Variables*/
	zona_comun->turno=1;
	zona_comun->feedback_Proc1=0;
	zona_comun->respuesta_Proc2=50;
	
	/*4. Lógica del programa*/
	srandom(getpid()); 
	rand=(int)(100.0*random()/RAND_MAX); // declarar núm. random entre 0 y 100
	printf("(Proc1)El número aleatorio es %d (Proc2 no sabe aún) \n",rand);
	
	intento = zona_comun->respuesta_Proc2;
	
	while(intento!=rand)
		{
			while( zona_comun->turno==2) {/*busy waiting*/}
			/*5. Entra mi turno*/
			intento = zona_comun->respuesta_Proc2;
			
			if ( num_intentos <= 9)
			{
				if (intento<rand)
					{
					//printf("(Proc1)He recibido el mensaje de Proc2 = %d \n", intento);
					printf("(Proc1)El numero es mayor \n");
					zona_comun->feedback_Proc1 =-2;//Proc1 envía pistas, introducción a memoria compartida	
					}
				else if (intento>rand)
					{
					//printf("(Proc1)He recibido el mensaje de Proc2 = %d \n", intento);
					printf("(Proc1)El numero es menor \n");
					zona_comun->feedback_Proc1 =-4;//Proc1 envía pistas, introducción a memoria compartida	
					}
				if (intento==rand)
					{
					printf("(Proc1)Has adivinado! \n");
					zona_comun->feedback_Proc1 =-10;//Proc1 envía pistas, introducción a memoria compartida	
					zona_comun->turno=2;  // cedo el turno: 
					exit(EXIT_SUCCESS); //fin del programa
					}
			}
			else if ( num_intentos >= 10)
			{
				printf("(Proc1)Num de intentos agotado, se acaba el juego \n");
				zona_comun->turno=2; // cedo el turno: 
				exit(EXIT_SUCCESS); //fin del programa
			}
		num_intentos+=1;
		zona_comun->turno=2; 		// cedo el turno: 
		}
		
	shmctl(shmid, IPC_RMID,0); 	// Borrado de la zona de memoria compartida 
	if(res==-1)
		{
			perror("shmctl");
			exit(EXIT_FAILURE);
		}	
	exit(EXIT_SUCCESS);
}

		

