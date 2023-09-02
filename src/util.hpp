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
    unsigned int n;
    unsigned long totalCollisions;
    unsigned int totalGoals;
};

inline double randomDouble(double min, double max) {
    return (rand() / (double)RAND_MAX) * (max - min) + min;
}

inline double randomDouble() {
    return randomDouble();
}

inline int previous_power(int n) {
    while (n & n - 1) {
        n = n & n - 1;
    }

    return n;
}

inline int random_power(int n) {
    // 4 est le nombre minimal de participants à un tournois.
    int r = rand() % (n - 3) + 4;
    int p = 4;

    while (p * 2 < r) {
        p *= 2;
    }

    return p;
}

inline double angleRounded(double input) {
    input = fmod(input + M_PI, 2 * M_PI);
    if (input < 0) {
        input += 2 * M_PI;
    }

    return input - M_PI;
}

inline bool likelyness(double v) {
    return randomDouble(0, 1) < v;
}

std::ostream &operator<<(std::ostream &out, gameStatistics stats);
std::ostream &operator<<(std::ostream &out, gameInformations stats);
