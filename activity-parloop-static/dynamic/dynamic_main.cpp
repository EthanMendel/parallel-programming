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

  if (argc < 8) {
    std::cerr << "usage: " << argv[0] << " <functionid> <a> <b> <n> <intensity> <nbthreads> <granularity>" << std::endl;
  }
  int funcID = atoi(argv[1]);
  double a = atof(argv[2]);
  double b = atof(argv[3]);
  int64_t n = atoll(argv[4]);
  int intensity = atoi(argv[5]);
  int numThreads = atoi(argv[6]);
  int gran = atoi(arg[7]);

  if (funcID < 1 || funcID > 4) {
    std::cerr << "<functionid> must be 1, 2, 3, or 4" << std::endl;
    return -1;
  }
  else {
    auto sTime = std::chrono::system_clock::now();
    StaticLoop sl;
    sl.setThreads(numThreads);
    double lead = (b - a) / n;
    double sum = 0;
    sl.parfor<std::vector<double>>(0, n, 1, gran,
      [&](std::vector<double>& tls) -> void {
        tls.push_back(0);
      },
      [&](int j, std::vector<double>& tls, int k) -> void {
        double num = (a + j + .5) * (lead);
        if (funcID == 1) {
          tls.at(k) += f1(num, intensity);
        }
        else if (funcID == 2) {
          tls.at(k) += f2(num, intensity);
        }
        else if (funcID == 3) {
          tls.at(k) += f3(num, intensity);
        }
        else {
          tls.at(k) += f4(num, intensity);
        }
      },
        [&](int k, std::vector<double> tls) -> void {
        sum += tls.at(k);
      }
      );
    auto eTime = std::chrono::system_clock::now();
    std::chrono::duration<double> tTime = eTime - sTime;
    std::cerr << tTime.count();
    double output = lead * sum;
    std::cout << output;
    return 0;
  }
}
