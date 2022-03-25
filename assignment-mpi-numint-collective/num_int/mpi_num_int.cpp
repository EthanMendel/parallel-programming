#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <vector>

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


int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  if (argc < 6) {
    std::cerr << "usage: " << argv[0] << " <functionid> <a> <b> <n> <intensity>" << std::endl;
    MPI_Finalize();
    return -1;
  }
  int funcID = atoi(argv[1]);
  double a = atof(argv[2]);
  double b = atof(argv[3]);
  int64_t n = atoll(argv[4]);
  int intensity = atoi(argv[5]);

  int P,i;
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  MPI_Comm_rank(MPI_COMM_WORLD, &i);

  if (funcID < 1 || funcID > 4) {
    std::cerr << "<functionid> must be 1, 2, 3, or 4" << std::endl;
    MPI_Finalize();
    return -1;
  }
  auto sTime = std::chrono::system_clock::now();
  double lead = (b - a) / n;
  double sum = 0;
  int amt = int(n/P);
  int s = i*amt;
  int e = s+amt;
  if(i+1 == P){
    e=n;
  }
  for (unsigned int i = s;i < e;i++) {
    double num = (a + i + .5) * (lead);
    if (funcID == 1) {
      sum += f1(num, intensity);
    }
    else if (funcID == 2) {
      sum += f2(num, intensity);
    }
    else if (funcID == 3) {
      sum += f3(num, intensity);
    }
    else {
      sum += f4(num, intensity);
    }
  }
  std::vector<double> locOutput;
  locOutput.push_back(lead * sum);
  std::vector<double> output;
  output.push_back(0);
  MPI_Reduce(&(locOutput[0]),&(output[0]), 1,MPI_DOUBLE,MPI_SUM, 0, MPI_COMM_WORLD);
  auto eTime = std::chrono::system_clock::now();
  std::chrono::duration<double> tTime = eTime - sTime;
  if(i==0){
    std::cerr << tTime.count();
    std::cout << output.at(0);
  }
  MPI_Finalize();
  return 0;
}
