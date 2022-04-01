#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>

float genA (int row, int col) {
  if (row > col)
    return 1.;
  else
    return 0.;
}

float genx0 (int i) {
  return 1.;
}


void checkx (int iter, long i, float xval) {
  if (iter == 1) {
    float shouldbe = i;
    if (fabs(xval/shouldbe) > 1.01 || fabs(xval/shouldbe) < .99 )
      std::cout<<"incorrect : x["<<i<<"] at iteration "<<iter<<" should be "<<shouldbe<<" not "<<xval<<std::endl;
  }

  if (iter == 2) {
    float shouldbe =(i-1)*i/2;
    if (fabs(xval/shouldbe) > 1.01 || fabs(xval/shouldbe) < .99)
      std::cout<<"incorrect : x["<<i<<"] at iteration "<<iter<<" should be "<<shouldbe<<" not "<<xval<<std::endl;
  }
}

//perform dense y=Ax on an n \times n matrix
void matmul(float*A, float*x, float*y, long n) {
  for (long row = 0; row<n; ++row) {
    float sum = 0;
    
    for (long col = 0; col<n; ++col) {
      //sum += x[col] *A[row][col]
      sum += x[col] * A[row*n+col];
    }

    y[row] = sum;
  }
}

int main (int argc, char*argv[]) {
    MPI_Init(&argc, &argv);

  if (argc < 3) {
    std::cout<<"usage: "<<argv[0]<<" <n> <iteration>"<<std::endl;
  }

  bool check = true;
  
  long n = atol(argv[1]);
  long iter = atol(argv[2]);

  int P,i;
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  MPI_Comm_rank(MPI_COMM_WORLD, &i);
  
  int sqrtP = sqrt(P);
  int pRow = floor(double(i) / double(sqrtP));
  int pCol = i % sqrtP;

  std::cout<<"process "<<i<<" of "<<P<<" is block at\trow "<<pRow<<"\t\tcol "<<pCol<<std::endl;
  int NsqrtP = n/sqrtP;
  int sRow = NsqrtP * pRow;
  int sCol = NsqrtP * pCol;
  int eRow = sRow + NsqrtP;
  int eCol = sCol + NsqrtP;
  std::cout<<"\tresponsible for\t\trows\t\tcols\n\t\t\t\t"<<sRow<<" to "<<eRow<<"\t"<<sCol<<" to "<<eCol<<std::endl;

  //initialize data
  float* A = new float[NsqrtP*NsqrtP];

  for (long row = sRow; row<eRow; row++) {
    for (long col=sCol; col<eCol; col++) {
      A[row*n+col] = genA(row, col);
    }
  }

   for (long row = 0; row<n; row++) {
     for (long col=0; col<n; col++) {
       std::cout<<A[row*n+col]<<" ";
     }
     std::cout<<std::endl;
   }

  float* x = new float[NsqrtP];

  for (long i=sCol; i<eCol; ++i)
    x[i] = genx0(i);

  // for (long i=0; i<n; ++i)
  //   std::cout<<x[i]<<" ";
  // std::cout<<std::endl;
  
  float* y = new float[NsqrtP];

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  for (int it = 0; it<iter; ++it) {
  
    matmul(A, x, y, NsqrtP);

    {
      float*t = x;
      x=y;
      y=t;
    }

    std::cout<<"\nx["<<it+1<<"]: ";
    for (long i=0; i<n; ++i)
      std::cout<<x[i]<<" ";
    std::cout<<std::endl;

    if (check)
      for (long i = 0; i<n; ++i)
	checkx (it+1, i, x[i]);
  }
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;

  std::cerr<<elapsed_seconds.count()<<std::endl;

  
  
  delete[] A;
  delete[] x;
  delete[] y;
  
  MPI_Finalize();
  return 0;
}
