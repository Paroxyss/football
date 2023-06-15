#include "Matrix.h"
#include <cmath>

double a_tanh(double x) {
    return tanh(x);
}

double ReLu(double x) {
    return fmax(0, x);
}

double sigmoide(double x) {
    return 1 / (1 + exp(-x));
}

double Heaviside(double x) {
    return (x < 0 ? 0 : 1);
}

void apply_activation(Matrix *c) {
    for (int i = 0; i < c->ligne; i++) {
        for (int j = 0; j < c->col; j++) {
            c->set(i, j, a_tanh(c->get(i, j)));
        }
    }
}
