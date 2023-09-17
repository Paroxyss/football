#include <ctime>
#include <random>
#include <thread>

thread_local std::mt19937
    rng(std::random_device{}() +
        std::hash<std::thread::id>()(std::this_thread::get_id()));
