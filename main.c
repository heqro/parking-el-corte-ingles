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

vehiculo_t * crearVehiculo(int espacio, char *id){
	
	vehiculo_t * vehiculoAux = malloc(sizeof(vehiculo_t));
	vehiculoAux->id = malloc(10*sizeof(char)); //10 porque son "matriculas" y no necesitamos mas
	vehiculoAux->espacio = espacio;
	vehiculoAux->id = strcpy(vehiculoAux->id, id);
    
	return vehiculoAux;
}
void mostrarVehiculo(vehiculo_t * vehiculoAux){
	
	if(vehiculoAux==NULL){
		printf("[0]");
	}else{
		printf("[%s]",vehiculoAux->id);
	}
}

typedef struct planta //nombre estructura
{
		
		vehiculo_t** plazas; //Vehiculos por planta
		int delimitador; //Delimita coches y camiones
		int nPlazas; //Plazas libres
		int topePlazas; //Total de plazas
			
} planta_t; //tipo dato estructura

planta_t * crearPlanta(int delimitador,int nPlazas){
	
	int i;
	
	planta_t * plantaAux = malloc(sizeof(planta_t));
	plantaAux->plazas = malloc(sizeof(vehiculo_t*) * nPlazas); //ESTO NO VA
	//Inicializamos vehiculos a NULL
	for(i = 0;i<nPlazas;i++){
		
		plantaAux->plazas[i]=NULL;
		
	}
	
	plantaAux->delimitador=delimitador;
	plantaAux->nPlazas=nPlazas;
	plantaAux->topePlazas=nPlazas;
	
	fprintf(stderr,"E1\n");
	return plantaAux;
}
void mostrarPlanta(planta_t * plantaAux){
	
	int i;
	for(i=0; i<plantaAux->topePlazas;i++){
		mostrarVehiculo(plantaAux->plazas[i]);
	}	
	
}


typedef struct parking //nombre estructura
{
		
		planta_t** plantas; //Plantas del parking
		int nPlantas; //Numero de plantas
					
} parking_t; //tipo dato estructura

parking_t * crearParking(int nPlazas ,int nPlantas, int nCoches, int nCamiones){
	
	parking_t * parkingAux = malloc(sizeof(parking_t));
    parkingAux->plantas = malloc(sizeof(planta_t) * nPlantas);
    // tenenmos n punteros de plantas
	int i;
	float pCamiones = (float) nCamiones/(float) ((float) nCoches + (float) nCamiones); //Tanto por 1 de Camiones
	int delimitador = (pCamiones*nPlazas); //Plazas asignadas camiones
	
	if(delimitador % 2 != 0 && delimitador!=nPlazas){
		delimitador++;
	}if(delimitador == 0 && nCamiones>0){
		delimitador = 2;
	}if(delimitador==10 && nCoches >0){
		delimitador = 8;
	}
	fprintf(stderr,"pCamiones : %f , delimitador : %i\n",pCamiones,delimitador);
	
	parkingAux->nPlantas = nPlantas;
	
	for(i = 0; i<nPlantas;i++)
		parkingAux->plantas[i] = crearPlanta(delimitador,nPlazas);
	printf("el valor de i es %i", i);
	return parkingAux;

}
void mostrarParking(parking_t * parkingAux){
	
	int i;
	
	printf("Parking :\n");
	for(i=parkingAux->nPlantas-1; i>=0;i--){
		printf("Planta %i",i);
		mostrarPlanta(parkingAux->plantas[i]);
		printf("\n");
	}	
	
}

volatile parking_t * aparcamiento;



int main(int argc, char **argv)
{
	int nCoches,nCamiones,nPlantas,nPlazas;
	
	nPlantas=1;
	nCamiones=0;

	switch(argc){ //Filto entrada
		case 1: fprintf(stderr,"Error: No hay parametros suficientes. \n");
				exit(1);
		
		case 5: nCamiones = atoi(argv[4]);
				if(nCamiones == 0){
					fprintf(stderr,"Error: No es el formato correcto. \n");
					exit(1);				
				}
		case 4: nCoches = atoi(argv[3]);
				if(nCoches == 0){
					fprintf(stderr,"Error: No es el formato correcto. \n");
					exit(1);				
				}
				
		case 3: nPlantas = atoi(argv[2]);
				if(nPlantas == 0){
					fprintf(stderr,"Error: No es el formato correcto. \n");
					exit(1);				
				}
				
		case 2: nPlazas = atoi(argv[1]);
				if(nPlazas == 0){
					fprintf(stderr,"Error: No es el formato correcto. \n");
					exit(1);				
				}
				if(argc==2 || argc==3)
					nCoches=2*nPlazas*nPlantas;
				break;
				
		default: fprintf(stderr,"Error: Hay mas parametros de los necesarios. \n"); 
				 exit(1);				
				
	}
	fprintf(stderr,"nPlazas %i,nPlantas %i,nCoches %i,nCamiones %i\n",nPlazas,nPlantas,nCoches,nCamiones);
	aparcamiento = crearParking(nPlazas,nPlantas,nCoches,nCamiones);
	
	//crearParking(nCoches,nCamiones,nPlazas,nPlantas);
	
	//mutex
	//threads
	
	//while(1)
	
	
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}

