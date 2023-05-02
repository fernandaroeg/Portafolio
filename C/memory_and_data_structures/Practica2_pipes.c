#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*  
Master Universitario en Automatica y Robotica
Curso: Programacion Avanzada
Nombre: Maria Fernanda Rodriguez Eguibar
Matricula: M20274
OBJETIVO PRACTICA 2:
Programa con dos pipes,una para comunicación de padre a hijo, y otra de hijo a padre. El Padre debe adivinar un numero elegido por el hijo
(entre 0 y 256), enviará sus intentos al hijo a través de la pipe. El hijo contestará con un mensaje para decir si el numero es mayor, menor o si ha acertado. 
 */

int main()
{
	/*1. Declaracion de variables*/
  int flow1[2], flow2[2], status, nread;
  int rand;
  int guess=128; //primera adivinanza del padre es el número medio
  char buffer[100], buffer2[100];
  
  /* 2. Creacion de los pipes*/ 
  if(pipe(flow1) == -1) perror("pipe");
  if(pipe(flow2) == -1) perror("pipe");
  printf("Dos Pipes creadas! \n");

  /* 3. Creacion del proceso hijo */ 
  switch(fork())
	{
	case -1: 
	  perror("fork");
	  exit(EXIT_FAILURE);
	  
	  /* 4. Logica del hijo.*/ ///////////////////////////
	case 0: 
	if(close(flow2[1] == -1) ) perror("hijo close");  //cierre del descriptor de lectura del pipe 2 en el proceso hijo  														

	/*Declaración del número random dentro del hijo*/
	srandom(getpid()); 
	rand=(int)(256.0*random()/RAND_MAX); // x será un numero entre 0 y 256
	printf("Hijo: el número aleatorio  es %d \n",rand);
	
	/* Feedback de la adivinanza
	1. leer padre*/
	nread = read(flow1[0],buffer2,100); // Lectura de la información contenida en el pipe. 
		switch(nread)
			{ 
				case -1:
					perror("read"); 
					break;
				case 0:
					perror("EOF");
					exit(EXIT_SUCCESS);
				default:
					buffer2[nread]='\0';
					printf("Hijo: he leido %d bytes ('%s') \n",nread,buffer2);
					exit(EXIT_SUCCESS);
			}
	/*2. decidir
	3. impirmir mensaje*/

	if(close(flow1[0] == -1) ) perror("hijo close");  // Cierre del descriptor de escritura del pipe1 en el proceso hijo 
	
	/* 5 Lógica del padre */ 
	default:
	  if(close(flow1[1]) ==- 1) perror("padre close");  // Cierre del descriptor de lectura en el pipe1 en el proceso padre 
	  
	  
     // 5.1 escritura de propuesta al hijo
		  //convertir guess a char
		  sprintf(buffer2, "Padre: Yo creo que el numero es %d \n", guess);
		  
	      if(write(flow2[1],buffer2,strlen(buffer2)) == -1) perror("padre write"); /* El proceso padre escribe en el pipe */
	 
	 /*recibir su feedback 
	 nueva propuesta 
	 print */
	 
	 if(close(flow2[0]) == -1)  perror("padre close"); // Cierre del descriptor de escritura en el pipe2 en el proceso padre
	    
	  wait(&status); /* El proceso padre espera la finalizacion del proceso hijo */  
	  exit(EXIT_SUCCESS);	      
	}
}
