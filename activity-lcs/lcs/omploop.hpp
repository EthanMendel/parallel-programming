#ifndef __OMPLOOP__
#define __OMPLOOP__

#include <functional>

// That does not solve the static activity, but provide the same feature so it can be used as a replacement.
// You need to add -fopenmp to CXXFLAGS and LDFLAGS for this to work.
// set number of thread by called setNbThread.
class OmpLoop {

  int nbthread;
  int granularity;

public:

  OmpLoop()
    :nbthread(1), granularity(1)
  {}
  
  void setNbThread(int t) {
    nbthread = t;
  }

  void setGranularity(int g) {
    granularity = g;
  }

  template<typename TLS>
  void parfor (size_t beg1, size_t end1, size_t increment1,
               size_t beg2, size_t end2, size_t increment2,
               std::function<void(TLS&)> before,
               std::function<void(int, int, TLS&)> f,
               std::function<void(TLS&)> after
               ) {
#pragma omp parallel num_threads(nbthread)
    {
      TLS tls;
      before(tls);
      
#pragma omp for schedule(dynamic, granularity) 
      for (size_t i=beg1,j=beg2; i>end1||j<=end2; i+=increment1,j+=increment2) {
	f(i, j, tls);
      }
#pragma omp critical
      after(tls);
    }
  }

};

#endif
