#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <cmath>
#include "static_loop.cpp"
#include <mutex>


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

  if (argc < 7) {
    std::cerr << "usage: " << argv[0] << " <functionid> <a> <b> <n> <intensity> <nbthreads>" << std::endl;
  }
  int funcID = atoi(argv[1]);
  double a = atof(argv[2]);
  double b = atof(argv[3]);
  int64_t n = atoll(argv[4]);
  int intensity = atoi(argv[5]);
  int numThreads = atoi(argv[6]);

  if (funcID < 1 || funcID > 4) {
    std::cerr << "<functionid> must be 1, 2, 3, or 4" << std::endl;
    return -1;
  }
  else {
    auto sTime = std::chrono::system_clock::now();
    StaticLoop sl;
    int perThread = int(n / numThreads);
    double lead = (b - a) / n;
    double sum = 0;
    std::mutex m;
    for (unsigned int i = 0;i < numThreads;i++) {
      int start = i * perThread;
      int end = start + perThread - 1;
      if (i + 1 == numThreads) {
        end += n % numThreads;
      }
      sl.parfor<double>(start, end, 1,
        [&](double& tls) -> void {
          tls = 0;
        },
        [&](int i, double& tls) -> void {
          double num = (a + i + .5) * (lead);
          if (funcID == 1) {
            tls += f1(num, intensity);
          }
          else if (funcID == 2) {
            tls += f2(num, intensity);
          }
          else if (funcID == 3) {
            tls += f3(num, intensity);
          }
          else {
            tls += f4(num, intensity);
          }
        },
          [&](double tls) -> void {
	  std::lock_guard<std::mutex> lg(m);
          sum += tls;
        }
      );
    }
    auto eTime = std::chrono::system_clock::now();
    std::chrono::duration<double> tTime = eTime - sTime;
    std::cerr << tTime.count();
    double output = lead * sum;
    std::cout << output;
    return 0;
  }
}
