#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "omploop.hpp"
#include <math.h>
#include <chrono>

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

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  int * pr = new int [n+1];

  pr[0] = 0;
  float frac = n/numThreads;
  int num_in_each_process = ceil(frac);
  for(int i=0;i<numThreads;i++){
      int start_index = i * num_in_each_process;
      int hold = pr[start_index];
      pr[start_index] = 0;
      for(int j=0;j<num_in_each_process;j++){
          if(start_index + j + 1 <= n + 1){
	      //std::cout<<"adding "<<start_index + j<<std::endl;
	      //std::cout<<"\tprev "<<pr[start_index+j]<<"\tval "<<arr[start_index+j]<<std::endl;
              pr[start_index + j + 1] = pr[start_index + j] + arr[start_index + j];
          }
      }
      pr[start_index] = hold;
      //std::cout<<"\tend "<<pr[start_index + num_in_each_process]<<" at "<<start_index + num_in_each_process<<std::endl;
  }
  for(int i=1;i<numThreads;i++){
      int start_index = i * num_in_each_process + 1;
      int diff = pr[start_index - 1];
      for(int j=0;j<num_in_each_process;j++){
          if(start_index + j + 1 <= n + 1){
              //std::cout<<"adding "<<diff<<" from "<<start_index - 1<<" to index "<<start_index + j<<std::endl;
              pr[start_index + j] += int(diff);
          }
      }
  }

  //for(int i=0;i<n;i++){
  //  std::cout<<" "<<arr[i];
  //}
  //std::cout<<std::endl;
  //for(int i=0;i<n;i++){
  //  std::cout<<" "<<pr[i];
  //}
  //std::cout<<std::endl;

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cerr<<elapsed.count()<<std::endl;

  checkPrefixSumResult(pr, n);

  delete[] arr;
  delete[] pr;

  return 0;
}
