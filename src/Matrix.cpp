#include <cstring>
#include <fstream>
#include <iostream>

#include "Matrix.hpp"
#include "Mutation.hpp"
#include "Rand.hpp"

Matrix::Matrix(int ligne, int col) {
#ifdef MATRIXDEBUG
    if (ligne * col > MATRIX_SIZE) {
        throw std::invalid_argument(
            "Tentative de création d'une matrice trop grande");
    }
#endif
    this->ligne = ligne;
    this->col = col;
    this->t = new double[MATRIX_SIZE];
    memset(t, 0, sizeof(double));
    this->ct = new double[MATRIX_SIZE];
}

Matrix::~Matrix() {
    delete[] this->t;
    delete[] this->ct;
}

void Matrix::print() {
    std::cout << "m: " << this->ligne << " x " << this->col << std::endl;
    for (int i = 0; i < this->ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            std::cout << this->get(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

void Matrix::initialize() {
    for (int i = 0; i < this->ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            double x = random_double();

            this->set(i, j, x);
        }
    }
}

double Matrix::get(int i, int j) {
#ifdef MATRIXDEBUG
    if (i >= this->ligne || j >= this->col) {
        throw std::invalid_argument("Bad matrice get: ");
    }
#endif

    return this->t[i * this->col + j];
}

void Matrix::set(int i, int j, double x) {
#ifdef MATRIXDEBUG
    if (i >= this->ligne || j >= this->col) {
        throw std::invalid_argument("Bad matrice set");
    }
#endif

    this->t[i * this->col + j] = x;
}

void Matrix::mult_inv(Matrix &a) {
#ifdef MATRIXDEBUG
    if (this->ligne != a.col) {
        throw std::invalid_argument("mult_inv: taille incorrecte");
    }
#endif

    for (int i = 0; i < a.ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            this->ct[i * this->col + j] = 0;
            for (int k = 0; k < this->ligne; k++) {
                this->ct[i * this->col + j] +=
                    a.get(i, k) * this->get(k, j);
            }
        }
    }

    auto tmp = this->t;
    this->t = this->ct;
    this->ct = tmp;

    this->ligne = a.ligne;
}

void Matrix::write(std::ofstream &file) {
    file.write((char *)&this->col, sizeof(this->col));
    file.write((char *)&this->ligne, sizeof(this->ligne));

    for (int i = 0; i < this->ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            double v = this->get(i, j);
            file.write((char *)&v, sizeof(double));
        }
    }
}

Matrix *Matrix::read(std::ifstream &file) {
    int lignes;
    int colonnes;

    file.read((char *)&colonnes, sizeof(colonnes));
    file.read((char *)&lignes, sizeof(lignes));

    auto m = new Matrix(lignes, colonnes);

    for (int i = 0; i < m->ligne; i++) {
        for (int j = 0; j < m->col; j++) {
            file.read((char *)&m->t[i * colonnes + j], sizeof(double));
        }
    }

    return m;
}

void Matrix::clone(Matrix *source, Matrix *dest) {
#ifdef MATRIXDEBUG
    if (source->col != dest->col || source->ligne != dest->ligne) {
        throw std::logic_error("clone de matrices de mauvaise taille");
    }
#endif
    for (int i = 0; i < source->ligne; i++) {
        for (int j = 0; j < source->col; j++) {
            dest->set(i, j, source->get(i, j));
        }
    }
}
