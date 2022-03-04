#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "omploop.hpp"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

void merge(int * arr, int l, int mid, int r) {
  
#if DEBUG
  std::cout<<l<<" "<<mid<<" "<<r<<std::endl;
#endif

  // short circuits
  if (l == r) return;
  if (r-l == 1) {
    if (arr[l] > arr[r]) {
      int temp = arr[l];
      arr[l] = arr[r];
      arr[r] = temp;
    }
    return;
  }

  int i, j, k;
  int n = mid - l;
  
  // declare and init temp arrays
  int *temp = new int[n];
  for (i=0; i<n; ++i)
    temp[i] = arr[l+i];

  i = 0;    // temp left half
  j = mid;  // right half
  k = l;    // write to 

  // merge
  while (i<n && j<=r) {
     if (temp[i] <= arr[j] ) {
       arr[k++] = temp[i++];
     } else {
       arr[k++] = arr[j++];
     }
  }

  // exhaust temp 
  while (i<n) {
    arr[k++] = temp[i++];
  }

  // de-allocate structs used
  delete[] temp;

}

void mergesort(int * arr, int l, int r) {

  if (l < r) {
    int mid = (l+r)/2;
    mergesort(arr, l, mid);
    mergesort(arr, mid+1, r);
    merge(arr, l, mid+1, r);
  }

}

int main (int argc, char* argv[]) {
  
  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int numThreads = atoi(argv[2]);
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);

  //insert sorting code here.
  int numIter = ceil(log2(n));
  std::cout<<"running for "<<numIter<<std::endl;
  for(int i=0;i<=numIter;i++){
    int perThrd = pow(2,i);
    std::cout<<"iteration "<<i<<" has "<<perThrd<<" per thread"<<std::endl;
    for(int j=0;j<n+1;j++){
      int start = j*perThrd;
      int end = start + perThrd;
      if(start < 0 || end > n+1){
        continue;
      }
      int mid = (start + end)/2;
      std::cout<<"\tmerge starting at "<<start<<" ending at "<<end<<" with a mid at "<<mid<<std::endl;
      merge(arr,start,mid,end);
    }
  }


  
  checkMergeSortResult (arr, n);
  
  delete[] arr;

  return 0;
}
