#include <stdio.h>
#include <stdlib.h>

typedef struct elemento
{
    int someInt;
} elem_t;

elem_t* crearElem(int valor){
    elem_t* aux = malloc(sizeof(elem_t));
    aux->someInt = valor;
    return aux;
}

//asignar
void asignarElem(elem_t* elem1, elem_t elem2){
    elem1->someInt = elem2.someInt;
}

void mostrarElem(elem_t elem){
	printf("[%i]",elem.someInt);
}

typedef struct nodo
{
    elem_t elem;
    struct nodo* sig;
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

void mostrarCola(cola_t cola){
    nodo_t* ptrAux = cola.primero;
    while(ptrAux != NULL){
        mostrarElem(ptrAux->elem);
        ptrAux = ptrAux->sig;
    }
}

int main()
{
    cola_t myCola;
    crearColaVacia(&myCola);
    if(esColaVacia(myCola))
        printf("Saludos!");
    for(int i = -3; i < 32; i++){
        insertar(*crearElem(i),&myCola);
    }
    
    mostrarCola(myCola);
    return 0;
}
