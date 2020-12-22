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

typedef struct elemento
{
    int espacio; // 1 -> coche; 2 -> camión
    char* id; // matrícula vehículo
} elem_t;

elem_t* crearElem(int espacio, char* id){
    elem_t* aux = malloc(sizeof(elem_t));
    aux->id = malloc(10*sizeof(char)); //10 porque son "matriculas" y no necesitamos mas
	aux->espacio = espacio;
	aux->id = strcpy(aux->id, id);
    return aux;
}

void asignarElem(elem_t* elem1, elem_t elem2){
    elem1 = crearElem(elem2.espacio, elem2.id);
}

void mostrarElem(elem_t elem){
	printf("[%s]",elem.id);
}

typedef struct nodo
{
    elem_t elem;
    struct nodo* sig;
    //int cabreo;
} nodo_t;

typedef struct cola
{
    nodo_t* primero;
    nodo_t* ultimo;
} cola_t;

void crearColaVacia(cola_t* cola){
    cola->primero = NULL;
    cola->ultimo = NULL;
}

int esColaVacia(cola_t cola){
    return cola.primero == NULL;
}

void insertar(elem_t elem, cola_t* cola){
    nodo_t* nuevoNodo = malloc(sizeof(nodo_t));
    nuevoNodo->sig = NULL;
    asignarElem(&nuevoNodo->elem, elem);
    if(esColaVacia(*cola)){
        cola->primero = nuevoNodo;
        cola->ultimo = nuevoNodo;
    } else {
        cola->ultimo->sig = nuevoNodo;
        cola->ultimo = nuevoNodo;
    }
}

elem_t* getPrimero(cola_t cola){ // recibir REFERENCIA primer elemento
    if(!esColaVacia(cola))
        return &cola.primero->elem;
    return NULL;
}

void eliminarCabecera(cola_t* cola){
    nodo_t* ptrAux;
    if(!esColaVacia(*cola)){
        ptrAux = cola->primero;
        if(cola->primero == cola->ultimo) // solo hay un elemento
            crearColaVacia(cola);
        else
            cola->primero = cola->primero->sig;
        free(ptrAux);
    }
}

typedef struct planta //nombre estructura
{
		
		elem_t** plazas; //Vehiculos por planta
		int delimitador; //Delimita coches y camiones
		int nPlazas; //Plazas libres
		int topePlazas; //Total de plazas
			
} planta_t; //tipo dato estructura

planta_t * crearPlanta(int delimitador,int nPlazas){
	
	int i;
	
	planta_t * plantaAux = malloc(sizeof(planta_t));
	plantaAux->plazas = malloc(sizeof(elem_t*) * nPlazas); //ESTO NO VA
	//Inicializamos vehiculos a NULL
	for(i = 0;i<nPlazas;i++){
		
		plantaAux->plazas[i]=NULL;
		
	}
	
	plantaAux->delimitador=delimitador;
	plantaAux->nPlazas=nPlazas;
	plantaAux->topePlazas=nPlazas;
	return plantaAux;
}
void mostrarPlanta(planta_t * plantaAux){
	
	int i;
	for(i=0; i<plantaAux->topePlazas;i++){
		if(plantaAux->plazas[i]){
            mostrarElem(*plantaAux->plazas[i]);
        } else {
            printf("[ ]");
        }
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
	return parkingAux;

}
void mostrarParking(parking_t parkingAux){
	
	int i;
	
	printf("Parking :\n");
	for(i=parkingAux.nPlantas-1; i>=0;i--){
		printf("Planta %i",i);
		mostrarPlanta(parkingAux.plantas[i]);
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
	mostrarParking(*aparcamiento);
	//crearParking(nCoches,nCamiones,nPlazas,nPlantas);
	
	//mutex
	//threads
	
	//while(1)
	
	
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}

