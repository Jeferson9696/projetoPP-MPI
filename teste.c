#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Inicializa o ambiente MPI
    MPI_Init(&argc, &argv);

    // Obtém o número de processos
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Obtém o rank do processo
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Imprime uma mensagem de "Hello world" do processo atual
    printf("Hello world from processor %d out of %d processors\n", world_rank, world_size);

    // Finaliza o ambiente MPI
    MPI_Finalize();
    return 0;
}
