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
