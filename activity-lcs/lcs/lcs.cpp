#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "omploop.hpp"

#ifdef __cplusplus
extern "C" {
#endif

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);

#ifdef __cplusplus
}
#endif

void print2Dvec(std::vector<std::vector<int>> v){
  for(int i=0;i<v.size();i++){
    for(int j=0;v.at(i).size();j++){
      std::cout<<v.at(i).at(j)<<"\t";
    }
    std::cout<<std::endl;
  }
  std::cout<<std::endl;
}

int main (int argc, char* argv[]) {

  if (argc < 4) { std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int m = atoi(argv[1]);
  int n = atoi(argv[2]);

  // get string data 
  char *X = new char[m];
  char *Y = new char[n];
  generateLCS(X, m, Y, n);

  
  //insert LCS code here.

  std::vector<std::vector<int>> C;
  for(int i=0;i<m;i++){
    std::vector<int> c;
    for(int j=0;j<n;j++){
      c.push_back(0);
    }
    C.push_back(c);
  }

  for(int d=0;d<(m+n-1);d++){//diagnal number
    for(int i=d,j=0;i>=0||j<n;i--,j++){
      if(i >= m){
        i = m-1;
      }
      if(j >= n){
        j = n-1;
      }
      if(X[i] == Y[j]){
        C.at(i).at(j) = C.at(i-1).at(j-1) + 1;
      }else{
        C.at(i).at(j) = max(C.at(i-1).at(j),C.at(i).at(j-1));
      }
    }
  }
  print2Dvec(C);
  int result = C.at(m-1).at(n-1); // length of common subsequence


  checkLCS(X, m, Y, n, result);

  delete[] X;
  delete[] Y;

  return 0;
}
