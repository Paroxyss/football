#include "Matrix.h"
#include "util.hpp"
#include <stdexcept>

/*
    De ce que j'ai comprit il existe énormément de différents algorithmes de
   crossover et il est difficile de savoir lequel est le plus performant il faut
   donc en implémenter plusieurs et les comparer directement. Il faudrait donc
   compléter ce fichier au fur et à mesure
*/

Matrix *one_pointer_crossover(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol");

    int x = rand() % a->col;

    Matrix *c = (x > a->col / 2 ? a : b)->copy();
    Matrix *t = (x > a->col / 2 ? b : a);

    for (int i = x; i < a->col; i++) {
        for (int j = 0; j < a->ligne; j++) {
            c->set(j, i, t->get(j, i));
        }
    }

    return c;
}

Matrix *double_pointer_crossover(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol");

    int x1 = rand() % a->col;
    int x2 = rand() % a->col + x1;

    Matrix *c = (x2 - x1 > a->col / 2 ? b : a)->copy();
    Matrix *t = x2 - x1 > a->col / 2 ? a : b;

    for (int i = x1; i <= x2; i++) {
        for (int j = 0; j < a->ligne; j++) {
            c->set(j, i, t->get(j, i));
        }
    }

    return c;
}

Matrix *uniform_crossover(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol");

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
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol");

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
        throw std::invalid_argument("lol");

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
        throw std::invalid_argument("lol");

    int x = rand() % a->col;
    int y = rand() % a->ligne;
    Matrix *c = a->copy();

    for (int i = x; i < b->ligne; i++) {
        for (int j = y; j < b->col; j++) {
            c->set(i, j, b->get(i, j));
        }
    }

    return c;
}

// oui oui ça existe vraiment
Matrix *SAC(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol");

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
