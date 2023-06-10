#include "Matrix.h"
#include <stdexcept>

/*
    De ce que j'ai comprit il existe énormément de différents algorithmes de
   crossover et il est difficile de savoir lequel est le plus performant il faut
   donc en implémenter plusieurs et les comparer directement. Il faudrait donc
   compléter ce fichier au fur et à mesure
*/

// TODO: à chaque fois je veux laisser les matrices a et b inchangées,
// je ne suis pas sur de le faire ici.
Matrix *one_pointer_crossover(Matrix *a, Matrix *b) {
    if (a->ligne != b->ligne || a->col != b->col)
        throw std::invalid_argument("lol");

    int x = rand() % a->col;

    Matrix *c = x > a->col / 2 ? a : b;
    Matrix *t = x > a->col / 2 ? b : a;

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

    Matrix *c = x2 - x1 > a->col / 2 ? b : a;
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
