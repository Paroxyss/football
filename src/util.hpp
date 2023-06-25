#pragma once

#include <cmath>
#include <iostream>
#include <ostream>

struct gameInformations {
    unsigned int collisions;
    unsigned int goals;
    double score;
};

struct gameStatistics {
    unsigned long totalCollisions;
    unsigned int totalGoals;
    double scoreMean;
    double goalsMean;
    double collisionsMean;
};

inline double randomDouble(double min, double max) {
    return (rand() / (double)RAND_MAX) * (max - min) + min;
}

inline double randomDouble() {
    return randomDouble(-1, 1);
}

inline double mutation_scalar() {
    double x = randomDouble(1, 5);
    return 0.5 * (tanh(1.75 * (x - 1)) + tanh(2 * (x - 4))) + 1;
}

inline int previous_power_2(int n) {
    while (n & n - 1) {
        n = n & n - 1;
    }

    return n;
}

inline double angleRounded(double input) {
    // on ajoute 2pi pour être dans les positifs et ajour un comportement plus
    // logique
    int offset = round(abs(input) / (2 * M_PI));
    // correction du signe
    offset *= abs(input) / input;
    // retour de l'angle corrigé
    return input - offset * 2 * M_PI;
}

std::ostream &operator<<(std::ostream &out, gameStatistics stats);
std::ostream &operator<<(std::ostream &out, gameInformations stats);
