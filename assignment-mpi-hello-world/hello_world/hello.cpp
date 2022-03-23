#include <mpi.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char*argv[]) {
  MPI_Init(&argc, &argv);

  int P,i,len;
  char name[MPI_MAX_PROCESSOR_NAME];

  MPI_Comm_size(MPI_COMM_WORLD, &P);
  MPI_Comm_rank(MPI_COMM_WORLD, &i);
  MPI_Get_processor_name(name,&len);

  std::cout<<"I am process "<<i<<" out of "<<P<<". I am running on "<<name<<std::endl;

  MPI_Finalize();
}
