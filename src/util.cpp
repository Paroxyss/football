#include "util.hpp"
#include <cmath>
#include <random>

std::random_device rd;  // a seed source for the random number engine
std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
 
// Generateur aléatoire
std::uniform_real_distribution<double> unif(-1, 1);
std::default_random_engine re;

// genère un double aléatoire
double randomDouble(double min, double max) {
    return (rand() / (double)RAND_MAX) * (max - min) + min;
}
double randomDouble() {
	return randomDouble(-1, 1);
}

std::uniform_real_distribution<double> unif5(0, 5);
std::default_random_engine re5;

double mutation_scalar() {
    int x = unif5(re5);
    return 0.5 * (tanh(1.75 * (x - 1)) + tanh(2 * (x - 4)) + 2);
}
