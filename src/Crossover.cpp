#include "Matrix.h"

#include "util.hpp"
#include <stdexcept>

Matrix *one_pointer_crossover(Matrix &a, Matrix &b) {
    if (a.ligne != b.ligne || a.col != b.col)
        throw std::invalid_argument("lol opc");

    int x = rand() % a.col;
    Matrix *c = new Matrix(a.ligne, a.col);

    for (int j = 0; j < a.col; j++) {
        Matrix &t = j < x ? a : b;

        for (int i = 0; i < a.ligne; i++) {
            c->set(i, j, t.get(i, j));
        }
    }

    return c;
}

Matrix *uniform_crossover(Matrix &a, Matrix &b) {
    if (a.ligne != b.ligne || a.col != b.col)
        throw std::invalid_argument("lol uc");

    Matrix *c = new Matrix(a.ligne, a.col);

    for (int i = 0; i < a.ligne; i++) {
        for (int j = 0; j < a.col; j++) {
            double x = (likelyness(0.5) ? a : b).get(i, j);
            c->set(i, j, x);
        }
    }

    return c;
}
