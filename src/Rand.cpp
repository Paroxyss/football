#include <ctime>
#include <random>
#include <thread>

thread_local std::mt19937
    rng(std::random_device{}() +
        std::hash<std::thread::id>()(std::this_thread::get_id()));

/**
 * Apparemment la fonction rand() n'est pas "thread-safe" et peut
 * sortir plusieurs fois le même nombre lorsque celui-ci est sollicité
 * simultanément sur plusieurs thread.
 *
 * ATTENTION LES DEUX BORNES SONT INCLUES
 */
int thrand(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}

double random_double(double min, double max) {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(rng);
}

double random_double() {
    return random_double(-1, 1);
}

bool likelyness(double v) {
    return random_double(0, 1) < v;
}
