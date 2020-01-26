#include <iostream>
#include <cstdlib>
#include <semaphore.h> // Semaforos
#include <pthread.h> // Hebras

using namespace std;

// CONSTANTES
const int num_items=75;
const int tam_vec=15;

// VARIABLES GLOBALES
int primera_libre=0; // indice del buffer que nos indica la primera posicion libre para escribir
int vec[tam_vec]; // vec => buffer donde se almacenan los datos producidos

// SEMAFOROS
sem_t puede_producir; // Semaforo que controla al productor
sem_t puede_consumir; // Semaforo que controla al consumidor
sem_t mutex; // Semaforo para la exclusion mutua (para que no se solapen procesos)

// PRODUCIR DATO
int producir_dato(){
	static int contador = 1;
	cout << "Dato producido: " << contador << endl;
	return contador++;
}
// CONSUMIR DATO
void consumir_dato(int dato){
	cout << "Dato recibido: " << dato << endl;
}
// EJECUTA LA HEBRA PRODUCTORA
void * productor(void *){
	
	for(unsigned i=0; i<num_items; i++){
	 
		int dato=producir_dato();
	 
		 sem_wait(&puede_producir);
	 
	 	// Exclusion mutua mediante el semaforo "mutex"
	 	sem_wait(&mutex);
	 	vec[primera_libre]=dato;
	 	primera_libre++;
	 	sem_post(&mutex);
	 
	 	sem_post(&puede_consumir);	 
	}	
	return NULL;	
}
// EJECUTA LA HEBRA CONSUMIDORA
void * consumidor (void *){
	for(unsigned i=0; i<num_items; i++){
		sem_wait(&puede_consumir); 
		// Exclusion mutua mediante el semaforo "mutex"
		sem_wait(&mutex);
		int dato=vec[primera_libre-1];
		primera_libre--;
		sem_post(&mutex);
		consumir_dato(dato);
		sem_post(&puede_producir);			
	}	
	return NULL;
}
// Punto de entrada al programa
int main(int argc, char *argv[])
{
    // Inicializamos los semáforos
    sem_init(&puede_producir, 0, tam_vec); // inicialmente se puede producir (hasta el tam_vec)
    sem_init(&puede_consumir, 0, 0); // inicialmente no se puede consumir
    sem_init(&mutex, 0, 1); // semaforo para EM: inicializado a 1, se utiliza para el pintado

    // Creamos las hebras
    pthread_t hebra_productor, hebra_consumidor;

    pthread_create(&hebra_productor,NULL,productor,NULL);
    pthread_create(&hebra_consumidor,NULL,consumidor,NULL);


    // permite continuar a hebra_productor y hebra_consumidor
    pthread_join(hebra_productor, NULL);
    pthread_join(hebra_consumidor, NULL);

    // Destruimos los semaforos
    sem_destroy(&puede_producir);
    sem_destroy(&puede_consumir);
    sem_destroy(&mutex);


    // Fin del programa
    exit(0);

}
