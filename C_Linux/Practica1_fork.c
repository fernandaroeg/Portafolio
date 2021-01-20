#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
/*  
Master Universitario en Automatica y Robotica
Curso: Programacion Avanzada
Nombre: Maria Fernanda Rodriguez Eguibar
Matricula: M20274
OBJETIVO PRACTICA 1: 
Escribir un programa que crea DOS hijos. El programa deberá tener una variable Z. Cada uno de los hijos deberá cambiar el valor de Z y imprimirlo en pantalla. 
 */

int main()
{
	/*1. Declaracion de variables*/
	pid_t pid;
	int status, Z =  0;
  
	/*2. Información del proceso */
	printf("El ID del proceso es %d, el de su padre es la consola %d \n", getpid(),getppid());
	printf("Padre: El valor inicial de Z es %d\n", Z);
	
	 /*3. Creación del primer proceso hijo */
	switch(pid = fork())
		{
			case (pid_t) -1:
				perror("fork");
				exit(-1);
				
			case (pid_t) 0:
				printf("Hijo1: Hola, soy el primer hijo, tengo PID %d\n",getpid());
				Z = 4;
				printf("Hijo1: Modifique el valor de Z a %d\n",Z);
			break;
		
			default:
				printf("Padre: PID %d. Estoy esperando a que mi 1er hijo termine...\n",getpid());
				waitpid(pid,&status,0); /*wait(int *status) provoca una espera del proceso padre hasta que cualquier proceso hijo finalice.*/
				printf("Padre: mi primer hijo ya termino!\n");
				
				/*4. Creación del segundo proceso hijo */
				switch(pid = fork())
				{
					case (pid_t) -1:
						perror("fork");
						exit(-1);
						
					case (pid_t) 0:
						printf("Hijo2: Hola, soy el 2o hijo, tengo PID %d. \n",getpid());
						Z=8;
						printf("HIjo2: Modifique el valor de Z a %d\n", Z);
					break;
				
					default:
					printf("Padre: PID %d. Estoy esperando a que mi 2o hijo termine...\n",getpid());
					waitpid(pid,&status,0); /* wait(int *status) provoca una espera del proceso padre hasta que cualquier proceso hijo finalice.*/
					printf("Padre: mis dos hijos han terminado. El valor final de Z es %d\n",Z);
					break;
				}
					
			break;
		}	
 exit(0);
}

