#include "util.hpp"

inline double replacement() {
    return randomDouble(-10, 10);
}

inline double tanhmut(double x) {
    double t = randomDouble(1, 5);
    return x * (0.5 * (tanh(1.75 * (t - 1)) + tanh(2 * (t - 4))) + 1);
}
