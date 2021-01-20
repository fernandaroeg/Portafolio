/*  Master Universitario en Automatica y Robotica
Curso: Programacion Avanzada
Nombre: Maria Fernanda Rodriguez Eguibar
Matricula: M20274
OBJETIVO PRACTICA 3:
Programa para comunicación de padre a hijo a través de colas. El Padre debe adivinar un numero elegido por el hijo (entre 0 y 100), 
enviará sus intentos al hijo a través de la cola El hijo contestará con un mensaje para decir si el numero es mayor, menor o si ha acertado. 
 */
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
/* Definición de la clave de la cola */
#define Clave_cola 1

/* Función auxiliar que crea la cola, retornando el identificador */
int crearcola(int key)
{
	int msgqid;
	msgqid = msgget(key, IPC_CREAT|0666);
	switch(msgqid)
	{
		case -1:
		perror("msgget");
		return(-1);
		
		default:
		return msgqid;
	}
} 

int main()
{
	/*1. Declaracion de variables*/
  int status, rand, intento, new, feedback;
  int id_cola, ret;
  int guess=50; //primera adivinanza del padre es el número medio
  int max=100;
  int min=0;
  
  /* 2. Creacion de la cola*/ 
	struct msgbuf{ long mtype;
						  char mtext[30];
						} mensaje;
						
	id_cola=crearcola(Clave_cola); //lamada a la función de creación de colas
	
		if(id_cola==-1)
	{
		printf("No se ha podido crear la cola !\n");
		exit(EXIT_FAILURE);
	}	
  printf("Cola creada! \n");

  switch(fork())  /* 3. Creacion del proceso hijo */ 
	{
	case -1: 
		perror("fork");
		exit(EXIT_FAILURE);
	case 0: 
		/* 4. Logica del hijo.*///////////////////////////////////////
		
		srandom(getpid()); 
		rand=(int)(100.0*random()/RAND_MAX); // declarar núm. random entre 0 y 100
		printf("(Hijo)El número aleatorio es %d (padre no sabe aún) \n",rand);
		
		while(1)
		{
			ret=msgrcv(id_cola,&mensaje,sizeof(mensaje.mtext),1,0);//Lectura de la información contenida en la cola
			if(ret == -1)
				{
					perror("msgrcv");
					exit(EXIT_FAILURE);
				}
			printf("(Hijo)He recibido el mensaje de mi padre = %s \n", mensaje.mtext);
			
			/* Lógica de la adivinanza*/
			intento = atoi(mensaje.mtext);
			mensaje.mtype=2; //inicialización de los campos de la estructura, hijo es tipo 2
			
			if (intento<rand)
				{
				printf("(Hijo)El numero es mayor \n");
				sprintf(mensaje.mtext, "%d",-2); 
				ret=msgsnd(id_cola,&mensaje,sizeof(mensaje.mtext),0); //hijo envía pistas, introducción a la cola
				}
			else if (intento>rand)
				{
				printf("(Hijo)El numero es menor \n");
				sprintf(mensaje.mtext, "%d",-4); 
				ret=msgsnd(id_cola,&mensaje,sizeof(mensaje.mtext),0); //hijo envía pistas, introducción a la cola	
				}
			if (intento==rand)
				{
				printf("(Hijo)Has adivinado! \n");
				sprintf(mensaje.mtext, "%d",-10); 
				ret=msgsnd(id_cola,&mensaje,sizeof(mensaje.mtext),0); //hijo envía pistas, introducción a la cola
				exit(EXIT_SUCCESS); //fin del programa
				}
		}

	default: /* 5 Lógica del padre *//////////////////////////////////////

		
		while(1)
		{
			sleep(0.1);
			mensaje.mtype=1; //inicialización de los campos de la estructura, padre es tipo 1
			sprintf(mensaje.mtext, "%d", guess); 
			ret=msgsnd(id_cola,&mensaje,sizeof(mensaje.mtext),0); //padre envía el msj, introducción a la cola
			
			//lectura del feedback del hijo
			ret=msgrcv(id_cola,&mensaje,sizeof(mensaje.mtext),2,0);//Lectura de la información contenida en la cola
			if(ret == -1)
				{
					perror("msgrcv");
					exit(EXIT_FAILURE);
				}
			
			if(atoi(mensaje.mtext) ==-2){ printf("(Padre)He recibido el feedback de mi hijo = mayor \n");}
			else if (atoi(mensaje.mtext) == -4) { printf("(Padre)He recibido el feedback de mi hijo = menor \n");} 
			else if (atoi(mensaje.mtext) == -10) { printf("(Padre)He recibido el feedback de mi hijo = adivinaste \n");} 
			
			feedback = atoi(mensaje.mtext);
			/* Lógica de la adivinanza*/
			if (feedback==-2)//mayor
				{
				guess=guess+1;
				/*min=guess;
				new=(max-min)/2;
				guess=guess+new;*/
				sprintf(mensaje.mtext, "%d",guess); 
				ret=msgsnd(id_cola,&mensaje,sizeof(mensaje.mtext),0); //padre corrije adivinanza, introducción a la cola
				printf("(Padre)Mi nueva propuesta es %d \n", guess);
				}
			else if (feedback==-4)//menor
				{
				guess=guess-1;
				/*max=guess;
				new=(max-min)/2;
				guess=guess-new;*/
				sprintf(mensaje.mtext, "%d",guess); 
				ret=msgsnd(id_cola,&mensaje,sizeof(mensaje.mtext),0); //padre corrije adivinanza, introducción a la cola
				printf("(Padre)Mi nueva propuesta es %d \n", guess);
				}
			else if (feedback ==-10)//igual
				{
				printf("(Padre)Adiviné! termina el juego \n");
				exit(EXIT_SUCCESS);
				}
		}
		
	  wait(&status); /* El proceso padre espera la finalizacion del proceso hijo */  
	  exit(EXIT_SUCCESS);	      
	}
		
	//elimnar la cola del sistema
	ret=msgctl(id_cola,IPC_RMID,(struct msqid_ds*)0);
	if(ret==-1) 
		perror("msgctl");
	else
		printf("La cola se ha cerrado \n");
}
