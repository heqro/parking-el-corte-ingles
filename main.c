#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct vehiculo //nombre estructura
{
	
	int espacio; //1 si es coche 2 si es camion
	char* id; //String con la matricula del vehiculo
	
} vehiculo_t; //tipo dato estructura

typedef struct planta //nombre estructura
{
		
		vehiculo_t* plazas; //Plazas por planta
		int delimitador; //Delimita coches y camiones
		int nPlazas; //Numero de plazas
			
} planta_t; //tipo dato estructura

typedef struct parking //nombre estructura
{
		
		planta_t* plazas; //Plazas por planta
		int nPlantas; //Numero de plantas
					
} parking_t; //tipo dato estructura

volatile parking_t aparcamiento;



int main(int argc, char **argv)
{
	int nCoches,nCamiones,nPlantas,nPlazas;
	
	nPlantas=1;
	nCamiones=0;
	
	
	
	
	switch(argc){
		case 1: fprintf(stderr,"Error: No hay parametros suficientes. \n");
				exit(1);
		
		case 5: nCamiones = atoi(argv[4]);
				if(nCamiones < 0){
					fprintf(stderr,"Error: No es el formato correcto. \n");
					exit(1);				
				}
		case 4: nCoches = atoi(argv[3]);
				if(nCoches < 0){
					fprintf(stderr,"Error: No es el formato correcto. \n");
					exit(1);				
				}
				
		case 3: nPlantas = atoi(argv[2]);
				if(nPlantas < 0){
					fprintf(stderr,"Error: No es el formato correcto. \n");
					exit(1);				
				}
				
		case 2: nPlazas = atoi(argv[1]);
				if(nPlazas < 0){
					fprintf(stderr,"Error: No es el formato correcto. \n");
					exit(1);				
				}
				if(argc==2 || argc==3)
					nCoches=2*nPlazas*nPlantas;
				break;
				
		default: fprintf(stderr,"Error: Hay mas parametros de los necesarios. \n"); 
				 exit(1);				
				
	}
	//crearParking();
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}

