#include "util.hpp"
#include "Rand.h"

/**
 * @brief Apparemment la fonction rand() n'est pas "thread-safe" et peut
 * sortir plusieurs fois le même nombre lorsque celui-ci est sollicité
 * simultanément sur plusieurs thread.
 *
 * ATTENTION LES DEUX BORNES SONT INCLUES
 */
int thrand(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}

double randomDouble(double min, double max) {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(rng);
}

double randomDouble() {
    return randomDouble(-1, 1);
}

int previous_power(int n) {
    while (n & n - 1) {
        n = n & n - 1;
    }

    return n;
}

int random_power(int n) {
    int r = thrand(4, n);
    int p = 4;

    while (p * 2 < r) {
        p *= 2;
    }

    return p;
}

double angleRounded(double input) {
    input = fmod(input + M_PI, 2 * M_PI);
    if (input < 0) {
        input += 2 * M_PI;
    }

    return input - M_PI;
}

bool likelyness(double v) {
    return randomDouble(0, 1) < v;
}

std::ostream &operator<<(std::ostream &out, gameStatistics stats) {
    out << "Stats{ collisions : "
        << "matches : " << stats.n << ", collisions : " << stats.totalCollisions
        << " | " << stats.totalCollisions / (double)(stats.n)
        << ", goals : " << stats.totalGoals << " | "
        << stats.totalGoals / (double)(stats.n) << " }";
    return out;
}

std::ostream &operator<<(std::ostream &out, gameInformations stats) {
    out << "Stats{ collisions : " << stats.collisions
        << ", goals : " << stats.goals << ", score : " << stats.score << " }";
    return out;
}
