#ifndef _THREADCLASS_H_
#define _THREADCLASS_H_

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <utility>

class threadclass {
public:
  ~threadclass() {
    if (_thread.joinable())
      _thread.join();
  }
  void mainProcess() {
    int n = 0;
    int m = 0;
    _thread = std::thread(&threadclass::f1, this, n, std::ref(m));
  }
  void f1(int n, int &m) {
    for (int i = 0; i != 5; ++i) {
      std::cout << "Thread 1 is executing\n";
      ++n;
      ++m;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

private:
  std::thread _thread;
};

#endif /* _THREADCLASS_H_ */
