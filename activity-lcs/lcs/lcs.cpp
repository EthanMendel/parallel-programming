#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "omploop.hpp"
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);

#ifdef __cplusplus
}
#endif

void print2Dvec(std::vector<std::vector<int>> v){
  std::cout<<"in print2Dvec"<<std::endl;
  for(int i=0;i<v.size();i++){
    for(int j=0;j<v.at(i).size();j++){
      std::cout<<v.at(i).at(j)<<" ";
    }
    std::cout<<std::endl;
  }
  std::cout<<std::endl;
  std::cout<<"ending print2Dvec"<<std::endl;
}

int main (int argc, char* argv[]) {

  if (argc < 4) { std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int m = atoi(argv[1]);
  int n = atoi(argv[2]);
  int numThreads = atoi(argv[3]);

  // get string data 
  char *X = new char[m];
  char *Y = new char[n];
  generateLCS(X, m, Y, n);

  
  //insert LCS code here.
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  std::vector<std::vector<int>> C;
  for(int i=0;i<=m;i++){
    std::vector<int> c;
    for(int j=0;j<=n;j++){
      c.push_back(0);
    }
    C.push_back(c);
  }

  OmpLoop omp;
  omp.setNbThread(numThreads);
  omp.setGranularity(n/numThreads);//is this right?

  for(int d=1;d<(m+n);d++){//diagnal number
    
    omp.parfor<int>(1,d+1,1,
      [&](int tls) -> void{},
      [&](int d2,int tls) -> void{
        //std::cout<<"diag #"<<d-1;
	int useI = d2;
        int useJ = d - d2 + 1;
        if(useI > m){
          useI = m;
        }else if(useI < 1){
          useI = 1;
        }
        if(useJ > n){
          useJ = n;
        }
        if(useI + useJ- 2 != d - 1){
          return;
        }
        //std::cout<<"("<<useI-1<<","<<useJ-1<<") ";
        if(X[useI-1] == Y[useJ-1]){
          C.at(useI).at(useJ) = C.at(useI-1).at(useJ-1) + 1;
        }else{
          C.at(useI).at(useJ) = std::max(C.at(useI-1).at(useJ),C.at(useI).at(useJ-1));
        }
        //std::cout<<std::endl;
      },
      [&](int tls) -> void{}
    );
  }
  //print2Dvec(C);
  int result = C.at(m).at(n); // length of common subsequence
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  checkLCS(X, m, Y, n, result);
  std::cerr<<elapsed.count()<<std::endl;

  delete[] X;
  delete[] Y;

  return 0;
}
