#include <stdio.h>
//#include <string.h>
#include <unistd.h>
#include <stdlib.h>
//#include <signal.h>
//#include <fcntl.h>
//#include <sys/wait.h>
//#include <errno.h>
#include <pthread.h>
//#include <semaphore.h>

//int cabreoCoches, cabreoCamiones;
pthread_mutex_t mutexAparcamiento;
pthread_mutex_t mutexCola;
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

int igualElem(elem_t elem1, elem_t elem2){
    return elem1.espacio == elem2.espacio && elem1.id == elem2.id;
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

void crearColaVacia(volatile cola_t* cola){
    cola->primero = NULL;
    cola->ultimo = NULL;
}

int esColaVacia(volatile cola_t cola){
    return cola.primero == NULL;
}

void insertar(elem_t elem, volatile cola_t* cola){
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

elem_t* getPrimero(volatile cola_t cola){ // recibir REFERENCIA primer elemento
    if(!esColaVacia(cola))
        return &cola.primero->elem;
    return NULL;
}

int soyPrimero(elem_t elem, volatile cola_t cola){
    elem_t* elemAux;
    if(!esColaVacia(cola)){
        elemAux = getPrimero(cola);
        return !igualElem(*elemAux, elem);
    }
    return 0;
}

void eliminarCabecera(volatile cola_t* cola){
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
		int nPlazas; // total de plazas por planta
		//int topePlazas; //Total de plazas
			
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
	//plantaAux->topePlazas=nPlazas;
	return plantaAux;
}
void mostrarPlanta(planta_t * plantaAux){
	
	int i;
	for(i = 0; i < plantaAux->nPlazas; i++){
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
	for(i = 0;i < parkingAux->nPlantas; i++){
		for(j = 0; j < (parkingAux->plantas[i]->nPlazas - 1);j++){
			if(parkingAux->plantas[i]->plazas[j]==NULL){
                if(flag == 1 || parkingAux->plantas[i]->plazas[j+1] == NULL){
					*plantaparking = i;
					*plazaparking = j;
					return 1;
				}
			}
		}
		if(parkingAux->plantas[i]->plazas[j] == NULL && flag == 1){
            // permitir que un coche entre al final de la planta
            *plantaparking = i;
            *plazaparking = j;
            return 1;
        }
	}
	
	return 0;
}

volatile parking_t * aparcamiento;
volatile cola_t colaEntrada;

void * accesoParkingCoches(void * id){
	
	elem_t * coche = crearElem(1,*((int *) id));
	int * plazaparking;
	int * plantaparking;
	
	plazaparking = malloc(sizeof(int));	
	plantaparking = malloc(sizeof(int));	
	
	while(1){
		sleep(rand() % 10 + 1);
        // Insertarnos en la cola de espera
        pthread_mutex_lock(&mutexCola);
        insertar(*coche, &colaEntrada);
        pthread_mutex_unlock(&mutexCola);
        while(!soyPrimero(*coche, colaEntrada));
        // Es nuestro turno, abandonamos la cola 
        
		pthread_mutex_lock(&mutexAparcamiento);
		while(encontrarPlazaLibre(aparcamiento,1,plantaparking,plazaparking)==0){
            pthread_cond_wait(&out,&mutexAparcamiento);
		} // Si salimos de este bucle, hemos conseguido sitio
		// Hemos encontrado un lugar, con lo que abandonamos la cola
		pthread_mutex_lock(&mutexCola);
        eliminarCabecera(&colaEntrada);
        pthread_mutex_unlock(&mutexCola);
        
		printf("Entrada Coche %i aparca en Planta %i y Plaza %i. ",coche->id,*plantaparking,*plazaparking);
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking] = coche;
		aparcamiento->plazasLibres = aparcamiento->plazasLibres - 1;
		printf("Plazas libres: %i\n", aparcamiento->plazasLibres);
		mostrarParking(*aparcamiento);
		pthread_mutex_unlock(&mutexAparcamiento);
		
		sleep(rand() % 10 + 1);
		
		pthread_mutex_lock(&mutexAparcamiento);
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking] = NULL;
		printf("Salida Coche %i aparca en Planta %i y Plaza %i. ",coche->id,*plantaparking,*plazaparking);
        aparcamiento->plazasLibres = aparcamiento->plazasLibres + 1;
		printf("Plazas libres: %i\n", aparcamiento->plazasLibres);
		pthread_cond_signal(&out);
		pthread_mutex_unlock(&mutexAparcamiento);
	}
}
void * accesoParkingCamiones(void * id){
	
	elem_t * camion = crearElem(2,*((int *) id));
	int * plazaparking;
	int * plantaparking;
	
	plazaparking = malloc(sizeof(int));	
	plantaparking = malloc(sizeof(int));	
			
	while(1){
		sleep(rand() % 10 + 1);
        // Insertarnos en la cola de espera
        pthread_mutex_lock(&mutexCola);
        insertar(*camion, &colaEntrada);
        pthread_mutex_unlock(&mutexCola);
        while(!soyPrimero(*camion, colaEntrada));
        // Es nuestro turno, abandonamos la cola 
		pthread_mutex_lock(&mutexAparcamiento);
		while(encontrarPlazaLibre(aparcamiento,2,plantaparking,plazaparking)==0){
            pthread_cond_wait(&out,&mutexAparcamiento);
		} //ya tiene acceso al mutex
		
		pthread_mutex_lock(&mutexCola);
        eliminarCabecera(&colaEntrada);
        pthread_mutex_unlock(&mutexCola);
		
		printf("Entrada Camion %i aparca en Planta %i y Plaza %i ",camion->id,*plantaparking,*plazaparking);
        aparcamiento->plazasLibres = aparcamiento->plazasLibres - 2;
		printf("Plazas libres: %i\n", aparcamiento->plazasLibres);
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking] = camion;
		aparcamiento->plantas[*plantaparking]->plazas[(*plazaparking) + 1] = camion;
		mostrarParking(*aparcamiento);
		pthread_mutex_unlock(&mutexAparcamiento);
		
		sleep(rand() % 10 + 1);
		
		pthread_mutex_lock(&mutexAparcamiento);
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking] = NULL;
		aparcamiento->plantas[*plantaparking]->plazas[*plazaparking + 1] = NULL;
		printf("Salida Camion %i aparca en Planta %i y Plaza %i. ",camion->id,*plantaparking,*plazaparking);
        aparcamiento->plazasLibres = aparcamiento->plazasLibres + 2;
		printf("Plazas libres: %i\n", aparcamiento->plazasLibres);
        pthread_cond_signal(&out);
		pthread_mutex_unlock(&mutexAparcamiento);
		
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
	aparcamiento = crearParking(nPlazas,nPlantas,nCoches,nCamiones);
    crearColaVacia(&colaEntrada);
	//mostrarParking(*aparcamiento);
	
	//mutex
	pthread_mutex_init(&mutexAparcamiento,NULL);
    pthread_mutex_init(&mutexCola,NULL);
	pthread_cond_init(&out,NULL);
	
	//threads
	thCoches = malloc(nCoches*sizeof(pthread_t*));
	thCamiones = malloc(nCamiones*sizeof(pthread_t*));
	idCoches = malloc(nCoches*sizeof(int));
	idCamiones = malloc(nCamiones*sizeof(int));
	for(i=0;i<nCoches;i++){
		idCoches[i] = i;	
		thCoches[i] = malloc(sizeof(pthread_t));
		pthread_create(thCoches[i],NULL,accesoParkingCoches,(void *)&idCoches[i]);
		
	}
	
	for(j=0;j<nCamiones;j++){
		idCamiones[j] = j+nCoches;	
		thCamiones[j] = malloc(sizeof(pthread_t));
		pthread_create(thCamiones[j],NULL,accesoParkingCamiones,(void *)&idCamiones[j]);
	}
	 
	while(1);
	return 0;
}

