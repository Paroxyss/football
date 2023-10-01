#include <iostream>
#include <mutex>
#include <ostream>
#include <queue>
#include <thread>

template <class T> class SafeQueue {
  public:
    void push(const T &val) {
        std::lock_guard<std::mutex> lock(_m);
        _q.push(val);
    }
    void pushReserved(const T &val) {
        std::lock_guard<std::mutex> lock(_m);
        reserved--;
        _q.push(val);
    }

    bool pop(T &val) {
        std::lock_guard<std::mutex> lock(_m);
        if (!_q.empty()) {
            val = _q.front();
            _q.pop();
            return true;
        } else {
            return false;
        }
    }
    int size() {
        std::lock_guard<std::mutex> lock(_m);
        return _q.size() + reserved;
    }
    int reserve() {
        std::lock_guard<std::mutex> lock(_m);
        reserved++;
        return _q.size() + reserved;
    }
    void clearReservations() {
        std::lock_guard<std::mutex> lock(_m);
        reserved = 0;
    }

  private:
    std::mutex _m;
    std::queue<T> _q;
    int reserved = 0;
};
