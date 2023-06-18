#pragma once
#include "Matrix.h"
#include <cmath>

inline double a_tanh(double x) {
    return tanh(x);
}

inline double ReLu(double x) {
    return fmax(0, x);
}

inline double sigmoide(double x) {
    return 1 / (1 + exp(-x));
}

inline double Heaviside(double x) {
    return x >= 0;
}

inline void apply_activation(Matrix &c) {
    for (int i = 0; i < c.ligne; i++) {
        for (int j = 0; j < c.col; j++) {
            c.set(i, j, a_tanh(c.get(i, j)));
        }
    }
}
