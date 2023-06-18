#pragma once

#include <cmath>

inline double randomDouble(double min, double max) {
    return (rand() / (double)RAND_MAX) * (max - min) + min;
}

inline double randomDouble() {
    return randomDouble(-1, 1);
}

inline double mutation_scalar() {
    int x = randomDouble(0, 5);
    return 0.5 * (tanh(1.75 * (x - 1) + tanh(2 * (x - 4)) + 2));
}
