#include "util.hpp"
#include <cmath>
#include <random>

// genère un double aléatoire
double randomDouble(double min, double max) {
    return (rand() / (double)RAND_MAX) * (max - min) + min;
}
double randomDouble() {
    return randomDouble(-1, 1);
}

double mutation_scalar() {
    int x = randomDouble(0, 5);
    return 0.5 * (tanh(1.75 * (x - 1)) + tanh(2 * (x - 4)) + 2);
}
