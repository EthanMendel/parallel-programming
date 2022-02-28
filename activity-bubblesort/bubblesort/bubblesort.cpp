#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "omploop.hpp"
#include <mutex>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (const int* arr, size_t n);

#ifdef __cplusplus
}
#endif

void swap(int* arr, int i, int j) {
  int temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}

int main (int argc, char* argv[]) {
  if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int numThreads = atoi(argv[2]);
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);

  //insert sorting code here.
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  // OmpLoop omp;
  // omp.setNbThread(numThreads);
  // omp.setGranularity(2);

  int swapped = 1;
  std::mutex swappedMutex;
  while (swapped) {
    swapped = 0;
    for(int i=0;i<n;i+=2){
      if (arr[i-1] > arr[i]) {
        swap(arr, i-1, i);
        swapped = 1;
      }
    }
    for(int i=1;i<n-2;i+=1){
      if (arr[i-1] > arr[i]) {
        swap(arr, i-1, i);
        swapped = 1;
      }
    }
    n--;
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elpased_seconds = end-start;
  //end of code

  
  checkMergeSortResult (arr, n);
  std::cerr<<elpased_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
