#include "util.hpp"
#include <cmath>
#include <random>

// Generateur aléatoire
std::uniform_real_distribution<double> unif(-1, 1);
std::default_random_engine re;

// genère un double aléatoire
double randomDouble() {
    double a = unif(re);
    return a;
}

std::uniform_real_distribution<double> unif5(0, 5);
std::default_random_engine re5;

double mutation_scalar() {
    int x = unif5(re5);
    return 0.5 * (tanh(3 * x - 12) + tanh(3 * x - 3) + 2);
}
