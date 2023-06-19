#include "Matrix.h"
#include "util.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>

Matrix::Matrix(int ligne, int col) {
    this->ligne = ligne;
    this->col = col;
    this->t = new double *[ligne];

    for (int i = 0; i < ligne; i++) {
        this->t[i] = (double *) calloc(col, sizeof(double));
    }
}

Matrix::~Matrix() {

    for (int i = 0; i < this->ligne; i++) {
        delete[] t[i];
    }

    delete[] t;
}

void Matrix::print() {
    std::cout << "Matrice : " << this->ligne << "x" << this->col << std::endl;
    for (int i = 0; i < this->ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            std::cout << this->t[i][j] << " ";
        }
		std::cout << std::endl;
    }
}

void Matrix::randomize() {
    for (int i = 0; i < this->ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            this->set(i, j, randomDouble() * 10);
        }
    }
}

/*
    Chaque coefficients d'une matrice mutée à une probabilité de 1/l d'être
   modifié où l est le nombre total de coefficients (ici, m->ligne * m->col)
   de manière qu'en "moyenne", seul 1 coefficient de la matrice ne soit
   modifié.

   mutation_scalar() retourne un coefficient multiplicateur comprit
   entre 0 et 2. Seulement, mutation_scalar() à une forte probabilité d'être
   proche de 1 il faudrait donc possiblement altérer plus de coefficients de la
   matrice pour accélérer l'exploration de l'ensemble solution.
*/
Matrix *mutation(Matrix *m) {
    Matrix *muted = new Matrix(m->ligne, m->col);
    int np = m->ligne * m->col;

    for (int i = 0; i < m->ligne; i++) {
        for (int j = 0; j < m->col; j++) {
            if ((rand() % np) != 0) {
                continue;
            }

            muted->set(i, j, m->get(i, j) * mutation_scalar());
        }
    }

    return muted;
}
