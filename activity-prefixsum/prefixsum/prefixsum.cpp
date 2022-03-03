#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "omploop.hpp"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  
  int n = atoi(argv[1]);
  int numThreads = atoi(argv[2]);

  int * arr = new int [n];
  generatePrefixSumData (arr, n);

  int * pr = new int [n+1];

  pr[0] = 0;
  float frac = n/numThreads;
  int num_in_each_process = ceil(frac);
  for(int i=0;i<numThreads;i++){
      int start_index = i * num_in_each_process;
      pr[start_index] = 0;
      for(int j=0;j<num_in_each_process;j++){
          if(start_index + j + 1 < n){
              pr[start_index + j + 1] = pr[start_index + j] + arr[start_index + j];
          }
      }
  }
  for(int i=1;i<numThreads;i++){
      int start_index = i * num_in_each_process;
      int diff = pr[start_index - 1];
      for(int j=0;j<num_in_each_process;j++){
          if(start_index + j + 1 < n){
              pr[start_index + j] += diff;
          }
      }
  }
  
  checkPrefixSumResult(pr, n);

  delete[] arr;
  delete[] pr;

  return 0;
}
