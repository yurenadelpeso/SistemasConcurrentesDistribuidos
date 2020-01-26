#include <iostream>
#include <ctime>
#include <cstdlib>
#include <unistd.h> // Funciones para poder llamar sleep()
#include "mpi.h"

using namespace std;

// Atributos
#define camarero  10 // Es el numero del proceso
//etiquetas
#define soltar     0
#define coger      1
#define levantarse 3
#define sentarse   2


// Metodos
void Filosofo (int id, int nprocesos); // Codigo proc. Filosofo
void Tenedor  (int id, int nprocesos); // Codigo proc. Tenedor
void Camarero (int id, int nprocesos); // Codigo proc. Camarero


int main(int argc, char** argv)
{
    // Inicialización de open-mpi
    int rank,size;
    srand(time(0));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Controlamos que estemos llamando correctamente al programa
    if (size != 11)
    {
        if (rank == 0) // Solo mostramos el mensaje 1 vez
        cout << "El numero de procesos debe ser 11" << endl;
        MPI_Finalize();
        return 0;
    }

    if (rank == 10)
        Camarero(rank, size); // El proceso 10 es el camarero
    else if (rank%2 == 0)
        Filosofo(rank,size); // los pares son filosofos
    else
        Tenedor(rank,size); // los impares son tenedores


    // Finalización de open-mpi
    MPI_Finalize();

    // Fin del programa
    return 0;
}

void Filosofo(int id, int nprocesos)
{
    // Hay que tener en cuenta que ahora hay un proceso mas (el camarero)
    int izq = (id+1) % (nprocesos-1);
    int der = (id+nprocesos-2) % (nprocesos-1);
    MPI_Status status;
    while (true)
    {
        // El filosofo pide sentarse
        cout << "Filosofo " << id << " solicita sentarse "  << endl;
        MPI_Send(NULL, 0, MPI_INT, camarero, sentarse, MPI_COMM_WORLD);

        // El filosofo espera a que le digan que puede sentarse
        MPI_Recv(NULL, 0, MPI_INT, camarero, sentarse, MPI_COMM_WORLD, &status);
        cout  << "Filosofo " << id << " se sienta "  << endl;
        // El filosofo se sienta

        // solicita tenedor izquierdo
        cout << "Filosofo " << id << " solicita tenedor izq. " << izq  << endl;
        MPI_Ssend(NULL, 0, MPI_INT, izq, coger, MPI_COMM_WORLD);


        // solicita tenedor derecho
        cout << "Filosofo " << id << " coge tenedor der. " << der  << endl;
        MPI_Ssend(NULL, 0, MPI_INT, der, coger, MPI_COMM_WORLD);


        cout << "Filosofo " << id << " COMIENDO"  << endl;
        sleep((rand()%3)+1); // comiendo

        // suelta el tenedor izquierdo
        cout << "Filosofo "<<id<< " suelta tenedor izq. " << izq  << endl;
        MPI_Ssend(NULL, 0, MPI_INT, izq, soltar, MPI_COMM_WORLD);


        // suelta el tenedor derecho
        cout << "Filosofo " << id << " suelta tenedor der. " << der << endl;
        MPI_Ssend(NULL, 0, MPI_INT, der, soltar, MPI_COMM_WORLD);


        // el filosofo se levanta
        cout  << "Filosofo " << id << " se levanta "   << endl;
        MPI_Ssend(NULL, 0, MPI_INT, camarero, levantarse, MPI_COMM_WORLD );


        cout << "Filosofo " << id << " PENSANDO"  << endl;
        sleep((rand()%3)+1); // pensando
    }
}

void Tenedor(int id, int nprocesos)
{
    int buf, Filo;
    MPI_Status status;
    while (true)
    {
        // Espera un peticion desde cualquier filosofo vecino ...
        MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, coger, MPI_COMM_WORLD, &status);

        // Recibe la peticion del filosofo ...
        Filo=status.MPI_SOURCE; //calcula id filosofo a partir de status
        cout  << "Tenedor " << id << " recibe petición de " << Filo  << endl;

        // Espera a que el filosofo suelte el tenedor...
        MPI_Recv(&Filo, 1, MPI_INT, Filo, soltar, MPI_COMM_WORLD, &status);
        cout  << "Tenedor " << id << " recibe liberación de " << Filo << endl;
    }
}

void Camarero(int id, int nprocesos)
{
    int buf, filo_sentados=0;
    MPI_Status status;
    while (true)
    {
        if (filo_sentados < 4) // El maximo de filosofos comiendo es 4
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // Puede sentarse o levantarse
        else
            MPI_Probe(MPI_ANY_SOURCE, levantarse, MPI_COMM_WORLD, &status);  // Solo puede levantarse


        if (status.MPI_TAG == sentarse) // se le deja sentarse
        {
            buf=status.MPI_SOURCE;
            MPI_Recv( NULL, 0, MPI_INT, buf, sentarse, MPI_COMM_WORLD,&status);
            filo_sentados++;

            MPI_Send( NULL, 0, MPI_INT, buf, sentarse, MPI_COMM_WORLD);
            cout << "Filosofo " << buf << " se sienta. Hay " << filo_sentados << " filosofos sentados. " << endl;

        }
        if (status.MPI_TAG == levantarse) // Se levanta
        {
            buf=status.MPI_SOURCE;
            MPI_Recv( NULL, 0, MPI_INT, buf, levantarse, MPI_COMM_WORLD,&status);
            filo_sentados--;
            cout << "Filosofo " << buf << " se levanta. Hay " << filo_sentados << " filosofos sentados.  " << endl;

        }
    }

}


