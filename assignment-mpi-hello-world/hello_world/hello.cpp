#include <mpi.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char*argv[]) {
  MPI_Init(&argc, &argv);

  int P;
  int i;

  MPI_Comm_size(MPI_COMM_WORLD, &P);
  MPI_Comm_rank(MPI_COMM_WORLD, &i);

  std::cout<<"I am process "<<i<<" out of "<<P<<". I am running on "<<gethostname<<std::endl;

  MPI_Finalize();
}
