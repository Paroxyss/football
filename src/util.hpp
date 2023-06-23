#pragma once

#include <cmath>
#include <iostream>
#include <ostream>

inline double randomDouble(double min, double max) {
    return (rand() / (double)RAND_MAX) * (max - min) + min;
}

inline double randomDouble() {
    return randomDouble(-1, 1);
}

inline double mutation_scalar() {
    int x = randomDouble(1, 5);
    return 0.5 * (tanh(1.75 * (x - 1)) + tanh(2 * (x - 4))) + 1;
}

// http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
inline int next_power_of_two(int v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
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
