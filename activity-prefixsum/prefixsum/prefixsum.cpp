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

  OmpLoop omp;
  omp.setNbThread(numThreads);
  omp.setGranularity(n/numThreads);

  int * pr = new int [n+1];

  pr[0] = 0;
  float frac = int(n/numThreads);
  int num_in_each_process = frac;
  omp.parfor<int>(0,numThreads,1,
    [&](int tls)->void{},
    [&](int i, int tls)->void{
      int start_index = i * num_in_each_process;
      int end = start_index + num_in_each_process;
      if(i == numThreads - 1){
        end += n%numThreads + 1;
	      //std::cout<<"adding leftover, end now "<<end<<std::endl;
      }
      int hold = pr[start_index];
      pr[start_index] = 0;
      for(int j=0;j<end;j++){
          if(start_index + j + 1 <= n + 1){
            //std::cout<<"adding "<<start_index + j<<std::endl;
            //std::cout<<"\tprev "<<pr[start_index+j]<<"\tval "<<arr[start_index+j]<<std::endl;
            pr[start_index + j + 1] = pr[start_index + j] + arr[start_index + j];
          }
      }
      pr[start_index] = hold;
      //std::cout<<"\tend "<<pr[end]<<" at "<<end<<std::endl;
    },
    [&](int tls)->void{}
  );
  for(int i=1;i<numThreads;i++){
      int start_index = i * num_in_each_process + 1;
      int end = start_index + num_in_each_process;
      if(i == numThreads -1){
        end += n%numThreads + 1;
      }
      int diff = pr[start_index - 1];
      omp.parfor<int>(start_index,end,1,
        [&](int tls)->void{},
        [&](int j,int tls)->void{
          if(j + 1 <= n + 1){
            //std::cout<<"adding "<<diff<<" from "<<start_index - 1<<" to index "<<j<<std::endl;
            pr[j] += int(diff);
          }
        },
        [&](int tls)->void{}
      );
  }

  //for(int i=0;i<n;i++){
  //  std::cout<<" "<<arr[i];
  //}
  //std::cout<<std::endl;
  //for(int i=0;i<=n;i++){
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
