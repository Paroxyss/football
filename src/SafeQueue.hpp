#include <algorithm>
#include <iostream>
#include <mutex>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <thread>

template <class T> class SafeQueue {
  public:
    SafeQueue(unsigned int threadNumber = 0) {
        reservedIds.resize(threadNumber);
        std::fill(reservedIds.begin(), reservedIds.end(), false);
    }
    void push(const T &val) {
        std::lock_guard<std::mutex> lock(_m);
        _q.push(val);
    }
    void pushReserved(const T &val, int id) {
        std::lock_guard<std::mutex> lock(_m);
        if (!reservedIds[id]) {
            throw std::logic_error(
                "Tentative de résolution d'un id non réservé");
        } else {
            reserved--;
            reservedIds[id] = false;
        }
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
    int reserve(int id) {
        std::lock_guard<std::mutex> lock(_m);
        if (id < 0 || id > reservedIds.size() - 1) {
            throw std::logic_error(
                "Tentative de réservation d'un id trop grand");
        }
        if (reservedIds[id]) {
            throw std::logic_error(
                "Tentative de reservation d'un id déjà réservé");
        } else {
            reserved++;
            reservedIds[id] = true;
        }
        return _q.size() + reserved;
    }
    void clearReservations() {
        std::lock_guard<std::mutex> lock(_m);
        reserved = 0;
		std::fill(reservedIds.begin(), reservedIds.end(), false);
    }
	void cancelRes(int id){
        if (id < 0 || id > reservedIds.size() - 1) {
            throw std::logic_error(
                "Tentative de réservation d'un id trop grand");
        }
		std::lock_guard<std::mutex> lock(_m);
		if(reservedIds[id]){
			reservedIds[id] = false;
			reserved--;
		}
	}

  private:
    std::mutex _m;
    std::queue<T> _q;
    int reserved = 0;
    std::vector<bool> reservedIds;
};
