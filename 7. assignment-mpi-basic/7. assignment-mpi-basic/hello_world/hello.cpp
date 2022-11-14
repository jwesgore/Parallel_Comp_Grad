#include <mpi.h>
#include <unistd.h>
#include <limits.h>
#include <iostream>

int main(int argc, char*argv[]) {

 MPI_Init(&argc, &argv);
    
    int size, rank;
    
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::cout << "I am process " << rank
        << " out of " << size
        << ". I am running on " << hostname << std::endl;

    MPI_Finalize();
}
