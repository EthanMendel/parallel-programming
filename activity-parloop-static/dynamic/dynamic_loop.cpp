#ifndef __STATIC_LOOP_H
#define __STATIC_LOOP_H

#include <functional>
#include <thread>

class StaticLoop {
private:
  // @brief you will want to have class member to store the number of threads
  // add a public setter function for this member.
  int numThreads;
public:
  // @breif write setters here.
  void setThreads(int n) {
    this->numThreads = n;
  }


  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel
  void parfor(size_t beg, size_t end, size_t inc,
    std::function<void(int)> f) {
    for (size_t i = beg; i < end; i += inc) {
      f(i);
    }
  }

  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel.
  ///
  /// Each thread that participate in the calculation will have its
  /// own TLS object.
  ///
  /// Each thread will execute function before prior to any
  /// calculation on a TLS object.
  ///
  /// Each thread will be passed the TLS object it executed before on.
  ///
  /// Once the iterations are complete, each thread will execute after
  /// on the TLS object. No two thread can execute after at the same time.
  template<typename TLS>
  void parfor(size_t beg, size_t end, size_t increment, size_t gran,
    std::function<void(TLS&)> before,
    std::function<void(int, TLS&, int)> f,
    std::function<void(int, TLS&)> after
  ) {
    TLS tls;
    for (int i = 0;i < numThreads;i++) {
      before(tls);
    }
    std::vector<std::thread> threads;
    int lastProcessed = beg;
    std::mutex findStart;
    for (int i = 0;i < numThreads;i++) {
      std::thread thrd([&](TLS& tls, int k, std::function<void(int, TLS&, int)> fu)->void {
        findStart.lock();
        while(lastProcessed < end){
          int s = lastProcessed;
          lastProcessed = s + gran;
          findStart.unlock();
          int e = s + gran;
          for (int j = s;j < e;j++) {
            fu(j, tls, k);
          }
          findStart.lock();
        }
        findStart.unlock();
      }, std::ref(tls), i, f);
      threads.push_back(std::move(thrd));
    }
    for (auto& thrd : threads) {
      if (thrd.joinable()) {
        thrd.join();
      }
    }
    for (int i = 0;i < numThreads;i++) {
      after(i, tls);
    }
  }

};

#endif
