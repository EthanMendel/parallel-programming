#include <mpi.h>
#include <iostream>

int main (int argc, char* argv[]) {
  MPI_Init (&argc, &argv);

  if (argc < 2) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <value>"<<std::endl;
    return -1;
  }

  int size;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  float* arr = new float[1];
  arr[0] = atof(argv[1]);

  if (rank == 0) {
    MPI_Send(&(arr[0]), 1, MPI_INT, 1, 123, MPI_COMM_WORLD);
  }
  else if (rank == 1) {
    MPI_Status s;
    MPI_Recv(&(arr[0]), 1, MPI_INT, 0, 123, MPI_COMM_WORLD, &s);
  }

  if(rank == 0){
    MPI_Status s;
    MPI_Recv(&(arr[0]), 1, MPI_INT, 1, 321, MPI_COMM_WORLD, &s);
    std::cout<<arr[0]<<std::endl;
  }else{
    arr[0] += 2;
    MPI_Send(&(arr[0]), 1, MPI_INT, 0, 321, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
