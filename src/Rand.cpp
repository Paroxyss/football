#include <ctime>
#include <random>
#include <thread>

thread_local std::mt19937
    rng(std::time(nullptr) +
        std::hash<std::thread::id>()(std::this_thread::get_id()));
