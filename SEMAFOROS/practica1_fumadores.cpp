#include <cstdlib>
#include <iostream>
#include <pthread.h> // Hebras
#include <semaphore.h> // Semaforos
#include <time.h> // Incluye a time(....)
#include <unistd.h> // Incluye a usleep(...)
#include <stdlib.h> // Incluye a rand(...)
using namespace std;

// SEMAFOROS
sem_t sem_estanquero;   // Semáforo  del estanquero (inicializado a 1)
sem_t sem_fumador[3];   // Array de 3 semáforos para los fumadores (inicializados a 0)
sem_t mutex;            // Semáforo para la exclusión mutua
// FUNCION FUMAR
void * fumar(void *ih_void){
	// numero o indice de esta hebra
	unsigned long ih = (unsigned long)ih_void;

	    while (true){
		if(ih==0){
			sem_wait(&sem_fumador[ih]);
			cout << "El fumador " << ih << " recibe el ingrediente: Tabaco"<< endl;
		}
		else if(ih==1){
			sem_wait(&sem_fumador[ih]);
			cout << "El fumador " << ih << " recibe el ingrediente: Papel" << endl;
		}
		else if(ih==2){
			sem_wait(&sem_fumador[ih]);
			cout << "El fumador " << ih << " recibe el ingrediente: Cerillas" << endl;
		}
        	//  comenzamos a fumar, desbloqueamos al estanquero para que pueda repartir
       		 sem_post(&sem_estanquero);
        		sem_wait(&mutex);
        
		cout << "Comienza a fumar el fumador: " << ih << endl;
        		sem_post(&mutex);
		 const unsigned miliseg = 100U + (rand() % 1900U); 
		usleep( 1000U*miliseg );
		sem_wait(&mutex);
        		cout << "Termina de fumar el fumador: " << ih << endl << endl;
        		sem_post(&mutex);
	  }
}
// FUNCION PRODUCIR INGREDIENTE
void * producir(void *){
    
	while (true){	
   		 sem_wait(&sem_estanquero);
 		  // calcular un numero aleatorio entre 0 y 2
  		  unsigned int ingrediente = (rand() % 3U);
  		  sem_wait(&mutex);
  		  cout << "Producido el ingrediente: " << ingrediente << endl;
  		  sem_post(&mutex);

  		  sem_post(&sem_fumador[ingrediente]);
 	}
}	
int main(){
 	// Inicializa la semilla aleatoria
	 srand( time(NULL) );
   	// Inicializamos los semaforos
	sem_init(&mutex, 0, 1);         
  	sem_init(&sem_estanquero, 0, 1); // inicialmente se puede producir
    
	for(int i=0; i<3; i++){
		sem_init(&sem_fumador[i], 0, 0); // inicialmente no se puede fumar
 	}
   	 // Declaramos las hebras
   	 pthread_t hebra_estanquero, hebra_fumador1, hebra_fumador2, hebra_fumador3;

   	 // Inicializamos las hebras
   	 pthread_create(&hebra_estanquero,NULL,producir,NULL);
   	 pthread_create(&hebra_fumador1,NULL,fumar,(void *)0);
   	 pthread_create(&hebra_fumador2,NULL,fumar,(void *)1);
   	 pthread_create(&hebra_fumador3,NULL,fumar,(void *)2);

   	 // Lanza las hebras
   	 pthread_join(hebra_estanquero, NULL);
   	 pthread_join(hebra_fumador1, NULL);
   	 pthread_join(hebra_fumador2, NULL);
   	 pthread_join(hebra_fumador3, NULL);

   	 // Destruimos los semaforos
   	 sem_destroy(&sem_estanquero);
   	 sem_destroy(&mutex);
   	 
   	 for(int i=0; i<3; i++){
   	 	sem_destroy(&sem_fumador[i]);
 	}  
    exit(0);
}
