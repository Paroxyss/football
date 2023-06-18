#include "Matrix.h"
#include "util.hpp"
#include <iostream>
#include <ostream>
#include <stdexcept>

/*
    De ce que j'ai comprit il existe énormément de différents algorithmes de
   crossover et il est difficile de savoir lequel est le plus performant il faut
   donc en implémenter plusieurs et les comparer directement. Il faudrait donc
   compléter ce fichier au fur et à mesure
*/

Matrix *one_pointer_crossover(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol opc");

    int x = rand() % a->col;
    Matrix *c = new Matrix(a->ligne, a->col);

    for (int j = 0; j < a->col; j++) {
        Matrix &t = *(j < x ? a : b);

        for (int i = 0; i < a->ligne; i++) {
            c->set(i, j, t.get(i, j));
        }
    }

    return c;
}

Matrix *double_pointer_crossover(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol dpc");

    int x1 = rand() % a->col;
    int x2 = rand() % a->col + x1;
    Matrix *c = new Matrix(a->ligne, a->col);

    for (int j = 0; j < a->col; j++) {
        Matrix *t = (x1 <= j && j <= x2 ? a : b);

        for (int i = 0; i < a->ligne; i++) {
            c->set(i, j, t->get(i, j));
        }
    }

    return c;
}

Matrix *uniform_crossover(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol uc");

    Matrix *c = new Matrix(a->ligne, a->col);

    for (int i = 0; i < a->ligne; i++) {
        for (int j = 0; j < a->col; j++) {
            double x = (rand() % 2 == 0 ? a : b)->get(i, j);
            c->set(i, j, x);
        }
    }

    return c;
}

Matrix *average_crossover(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col) {
        throw std::invalid_argument("erreur average_crossover");
    }

    Matrix *c = new Matrix(a->ligne, a->col);

    for (int i = 0; i < a->ligne; i++) {
        for (int j = 0; j < a->col; j++) {
            c->set(i, j, (a->get(i, j) + b->get(i, j)) / 2);
        }
    }

    return c;
}

Matrix *WARP(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol WARP");

    double x = (randomDouble() + 1) / 2;
    Matrix *c = new Matrix(a->ligne, a->col);

    for (int i = 0; i < a->ligne; i++) {
        for (int j = 0; j < a->col; j++) {
            c->set(i, j, (a->get(i, j) * x + b->get(i, j) * (1 - x)));
        }
    }

    return c;
}

Matrix *PMX(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol PMX");

    int x = rand() % a->col;
    int y = rand() % a->ligne;
    Matrix *c = new Matrix(a->ligne, a->col);

    for (int i = 0; i < a->ligne; i++) {
        for (int j = 0; j < a->col; j++) {
            bool r = i <= y || j >= x;
            c->set(i, j, (r ? a : b)->get(i, j));
        }
    }

    return c;
}

// oui oui ça existe vraiment
Matrix *SAC(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol SAC");

    switch (rand() % 6) {
    case 0:
        return one_pointer_crossover(a, b);
    case 1:
        return double_pointer_crossover(a, b);
    case 2:
        return uniform_crossover(a, b);
    case 3:
        return average_crossover(a, b);
    case 4:
        return WARP(a, b);
    case 5:
        return PMX(a, b);
    }

    return NULL;
}
