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

//int cabreoCoches, cabreoCamiones;
pthread_mutex_t mutex;
pthread_cond_t out;
typedef struct elemento
{
    int espacio; // 1 -> coche; 2 -> camión
    int id; // matrícula vehículo
} elem_t;

elem_t* crearElem(int espacio, int id){
    elem_t* aux = malloc(sizeof(elem_t));
    aux->espacio = espacio;
	aux->id = id;
    return aux;
}

void asignarElem(elem_t* elem1, elem_t elem2){
    elem1 = crearElem(elem2.espacio, elem2.id);
}

void mostrarElem(elem_t elem){
	printf("[%i]",elem.id);
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
		int nPlazas; //Plazas libres
		int topePlazas; //Total de plazas
			
} planta_t; //tipo dato estructura

planta_t * crearPlanta(int nPlazas){
	
	int i;
	
	planta_t * plantaAux = malloc(sizeof(planta_t));
	plantaAux->plazas = malloc(sizeof(elem_t*) * nPlazas); //ESTO NO VA
	//Inicializamos vehiculos a NULL
	for(i = 0;i<nPlazas;i++){
		
		plantaAux->plazas[i]=NULL;
		
	}
	
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
		int plazasLibres; //Plazas libres
		planta_t** plantas; //Plantas del parking
		int nPlantas; //Numero de plantas
					
} parking_t; //tipo dato estructura

parking_t * crearParking(int nPlazas ,int nPlantas, int nCoches, int nCamiones){
	
	int i;
	
	parking_t * parkingAux = malloc(sizeof(parking_t));
    parkingAux->plantas = malloc(sizeof(planta_t) * nPlantas);
    parkingAux->plazasLibres = nPlazas*nPlantas;
	parkingAux->nPlantas = nPlantas;
	for(i = 0; i<nPlantas;i++)
		parkingAux->plantas[i] = crearPlanta(nPlazas);
	return parkingAux;

}
void mostrarParking(parking_t parkingAux){
	
	int i;
	
	printf("Parking :\n");
	for(i=parkingAux.nPlantas-1; i>=0;i--){
		printf("Planta %i ",i);
		mostrarPlanta(parkingAux.plantas[i]);
		printf("\n");
	}	
	
}
int encontrarPlazaLibre(volatile parking_t * parkingAux, int flag, int * plantaparking, int * plazaparking ){ //1 coche 2 camion
	
	int i,j;
	
	for(i=0;i<parkingAux->nPlantas;i++){
		for(j=0; j< (parkingAux->plantas[i]->nPlazas);j++){
			if(parkingAux->plantas[i]->plazas[j]==NULL){
				if(flag == 1){
					*plantaparking = i;
					*plazaparking = j;
					fprintf(stderr,"i : %i j : %i \n",*plantaparking,*plazaparking);
					return 1;
				}else if(j+1<=parkingAux->plantas[i]->nPlazas && parkingAux->plantas[i]->plazas[j+1] == NULL){
					
					*plantaparking = i;
					*plazaparking = j;
					fprintf(stderr,"i : %i j : %i \n",*plantaparking,*plazaparking);
					return 1;
				}
			}
		}
	}
	
	return 0;
}

volatile parking_t * aparcamiento;

void * accesoParkingCoches(void * id){
	
	elem_t * coche = crearElem(1,*((int *) id));
	int * plazaparking;
	int * plantaparking;
	
	plazaparking = malloc(sizeof(int));	
	plantaparking = malloc(sizeof(int));	
	
	while(1){
		
		pthread_mutex_lock(&mutex);
		while(encontrarPlazaLibre(aparcamiento,1,plantaparking,plazaparking)==0){
		//	pthread_cond_wait(&out,&mutex);
		} //ya tiene acceso al mutex
		printf("Entrada Coche %i aparca en Planta %i y Plaza %i. ",coche->id,*plantaparking,*plazaparking);
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking] = coche;
		aparcamiento->plazasLibres = aparcamiento->plazasLibres - 1;
		printf("Plazas libres: %i\n", aparcamiento->plazasLibres);
		mostrarParking(*aparcamiento);
		pthread_mutex_unlock(&mutex);
		
		sleep(rand() % 10 + 1);
		
		pthread_mutex_lock(&mutex);
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking] = NULL;
		printf("Salida Coche %i aparca en Planta %i y Plaza %i. ",coche->id,*plantaparking,*plazaparking);
		printf("Plazas libres: %i\n", aparcamiento->plazasLibres);
		aparcamiento->plazasLibres = aparcamiento->plazasLibres + 1;
		//pthread_cond_signal(&out);
		pthread_mutex_unlock(&mutex);
	}
}
void * accesoParkingCamiones(void * id){
	
	elem_t * camion = crearElem(2,*((int *) id));
	int * plazaparking;
	int * plantaparking;
	
	plazaparking = malloc(sizeof(int));	
	plantaparking = malloc(sizeof(int));	
			
	while(1){
		
		pthread_mutex_lock(&mutex);
		while(encontrarPlazaLibre(aparcamiento,2,plantaparking,plazaparking)==0){
			fprintf(stderr,"E4\n");
	//		pthread_cond_wait(&out,&mutex);
		} //ya tiene acceso al mutex
		printf("Entrada Camion %i aparca en Planta %i y Plaza %i ",camion->id,*plantaparking,*plazaparking);
		printf("Plazas libres: %i\n", aparcamiento->plazasLibres);
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking] = camion;
		aparcamiento->plantas[*plantaparking]->plazas[(*plazaparking) + 1] = camion;
		aparcamiento->plazasLibres = aparcamiento->plazasLibres - 2;
		mostrarParking(*aparcamiento);
		pthread_mutex_unlock(&mutex);
		
		sleep(rand() % 10 + 1);
		
		pthread_mutex_lock(&mutex);
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking] = NULL;
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking + 1] = NULL;
		printf("Salida Camion %i aparca en Planta %i y Plaza %i. ",camion->id,*plantaparking,*plazaparking);
		printf("Plazas libres: %i\n", aparcamiento->plazasLibres);
		aparcamiento->plazasLibres = aparcamiento->plazasLibres + 2;
	//	pthread_cond_signal(&out);
		pthread_mutex_unlock(&mutex);
		
	}
}

int main(int argc, char **argv)
{
	int nCoches,nCamiones,nPlantas,nPlazas,i,j;
	pthread_t ** thCoches;
	pthread_t ** thCamiones;
	int * idCoches;
	int * idCamiones;
	
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
	//fprintf(stderr,"nPlazas %i,nPlantas %i,nCoches %i,nCamiones %i\n",nPlazas,nPlantas,nCoches,nCamiones);
	aparcamiento = crearParking(nPlazas,nPlantas,nCoches,nCamiones);
	mostrarParking(*aparcamiento);
	//crearParking(nCoches,nCamiones,nPlazas,nPlantas);
	//elem_t * coche = crearElem(1,"HEY");
	//aparcamiento->plantas[1]->plazas[1] = coche;
	//mostrarParking(*aparcamiento);
	//aparcamiento->plantas[1]->plazas[1] = NULL;
	//mostrarParking(*aparcamiento);
	
	//mutex
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&out,NULL);
	
	//threads
	thCoches = malloc(nCoches*sizeof(pthread_t*));
	thCamiones = malloc(nCamiones*sizeof(pthread_t*));
	idCoches = malloc(nCoches*sizeof(int));
	idCamiones = malloc(nCamiones*sizeof(int));
	fprintf(stderr,"E1\n");
	for(i=0;i<nCoches;i++){
		idCoches[i] = i;	
		thCoches[i] = malloc(sizeof(pthread_t));
		pthread_create(thCoches[i],NULL,accesoParkingCoches,(void *)&idCoches[i]);
		fprintf(stderr,"E2\n");
		
	}
	
	for(j=0;j<nCamiones;j++){
		idCamiones[j] = j+nCoches;	
		thCamiones[j] = malloc(sizeof(pthread_t));
		pthread_create(thCamiones[j],NULL,accesoParkingCamiones,(void *)&idCamiones[j]);
		fprintf(stderr,"E3\n");
	}
	 
	while(1);
	
	
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}

