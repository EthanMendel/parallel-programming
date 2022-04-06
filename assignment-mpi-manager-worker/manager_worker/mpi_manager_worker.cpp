#include <mpi.h>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

double doIntegration(int funcID, int a, int b, int n, int intensity,int s, int e){
  double lead = (b - a) / n;
  double sum = 0;
  for (unsigned int i = s;i < e;i++) {
    std::cout<<"\tdoing sum "<<i<<"\t currrent sum "<<sum<<std::endl;
    double num = (a + i + .5) * (lead);
    if (funcID == 1) {
      float a = f1(num, intensity);
      std::cout<<"\t\tfunc-1 gave "<<a<<std::endl;
      sum += a;
    } else if (funcID == 2) {
      sum += f2(num, intensity);
    } else if (funcID == 3) {
      sum += f3(num, intensity);
    } else {
      sum += f4(num, intensity);
    }
  }
  return sum;
}

int main (int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  if (argc < 6) {
    std::cerr<<"usage: mpirun -np <processes> "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    MPI_Finalize();
    return -1;
  }
  int funcID = atoi(argv[1]);
  double a = atof(argv[2]);
  double b = atof(argv[3]);
  int64_t n = atoll(argv[4]);
  int intensity = atoi(argv[5]);
  if (funcID < 1 || funcID > 4) {
    std::cerr << "<functionid> must be 1, 2, 3, or 4" << std::endl;
    MPI_Finalize();
    return -1;
  }
  int P, i;
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  MPI_Comm_rank(MPI_COMM_WORLD, &i);
  int gran = 10;
  double sum = 0;
  double* params = new double[7];
  int lastProcessed = 0;
  int numSent = 0;
  int numRecv = 0;
  if(i == 0){
    for(int i=1;i<P;i++){
      int s = lastProcessed;
      int e = s + gran;
      if(e > n){
        e = n;
      }
      lastProcessed = e;
      params[0] = funcID;
      params[1] = a;
      params[2] = b;
      params[3] = n;
      params[4] = intensity;
      params[5] = s;
      params[6] = e;
      MPI_Send(&(params[0]), 7, MPI_DOUBLE, i, 111, MPI_COMM_WORLD);
      numSent++;
    }
    while(numRecv < numSent){
      double* recArr = new double[1];
      MPI_Status status;
      MPI_Recv(&(recArr[0]), 1, MPI_DOUBLE, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);
      numRecv++;
      sum += recArr[0];
      if(lastProcessed < n){
        int s = lastProcessed;
        int e = s + gran;
        if(e > n){
          e = n;
        }
        lastProcessed = e;
        params[0] = funcID;
        params[1] = a;
        params[2] = b;
        params[3] = n;
        params[4] = intensity;
        params[5] = s;
        params[6] = e;
        MPI_Send(&(params[0]), 7, MPI_DOUBLE, status.MPI_SOURCE, 111, MPI_COMM_WORLD);
        numSent++;
      }else{
        MPI_Send(&(params[0]), 7, MPI_DOUBLE, status.MPI_SOURCE, 222, MPI_COMM_WORLD);
      }
    }
    std::cout<<sum<<std::endl;
  }else{
    MPI_Status status;
    MPI_Recv(&(params[0]), 7, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    while(status.MPI_TAG == 111){
      for(int i=0;i<7;i++){
	std::cout<<params[i]<<",";
      }
      std::cout<<std::endl;
      double* res = new double[1];
      res[0] = doIntegration(params[0],params[1],params[2],params[3],params[4],params[5],params[6]);
      std::cout<<"\n"<<res[0]<<std::endl;
      MPI_Send(&(res[0]), 1, MPI_DOUBLE, 0, 111, MPI_COMM_WORLD);
      MPI_Recv(&(params[0]), 7, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }
  }

  MPI_Finalize();
  return 0;
}
