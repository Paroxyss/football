#include "Matrix.h"
#include <iostream>

void Matrix::initialize() {
    this->t = new double *[ligne];

    for (int i = 0; i < ligne; i++) {
        double *v = new double[col];
        for (int j = 0; j < col; j++) {
            v[j] = 0;
        }

        this->t[i] = v;
    }
}

Matrix::Matrix(int ligne, int col) {
    this->ligne = ligne;
    this->col = col;
    initialize();
}

Matrix::~Matrix() {
    if (!(this->col == 0 && this->ligne == 0)) {
        for (int i = 0; i < ligne; i++) {
            delete[] t[i];
        }
        delete[] t;
    }
}

double Matrix::get(int i, int j) {
    if (i > this->ligne || j > this->col)
        std::cout << "BAD CASE" << std::endl;
    return this->t[i][j];
}

void Matrix::set(int i, int j, double x) {
    if (i > this->ligne || j > this->col)
        std::cout << "BAD CASE" << std::endl;
    this->t[i][j] = x;
}

double Matrix::trace() {
    if (this->col != this->ligne)
        throw std::invalid_argument("lol");

    double s = 0;

    for (int i = 0; i < this->col; i++) {
        s += this->get(i, i);
    }

    return s;
}

void Matrix::print() {
    std::cout << "Matrice : " << this->ligne << "x" << this->col << std::endl;
    for (int i = 0; i < ligne; i++) {
        for (int j = 0; j < col; j++) {
            std::cout << this->t[i][j] << " ";
        }
        std::cout << this->t[i] << "\n";
    }
}

void Matrix::transpose() {
    double **t = new double *[this->ligne];

    for (int i = 0; i < this->ligne; i++) {
        double *v = new double[this->col];
        for (int j = 0; j < this->col; j++) {
            v[j] = this->get(j, i);
        }

        t[i] = v;
    }

    this->t = t;
    int tmp = this->col;
    this->col = this->ligne;
    this->ligne = tmp;
}

// Multiplie deux matrices en place dans this (this * a)
void Matrix::mult(Matrix *a) { // PAS FONCTIONNEL + MEMORY LEAKS
    if (this->col != a->ligne) {
        throw std::invalid_argument("lol");
    }

    double **t = new double *[this->ligne];
    for (int i = 0; i < this->ligne; i++) {
        double *v = new double[a->col];
        for (int j = 0; j < a->col; j++) {
            v[j] = 0;
            for (int k = 0; k < this->ligne; k++) {
                v[j] += this->get(i, k) * a->get(k, j);
            }
        }
        t[i] = v;
    }
    this->t = t;
    this->col = a->col;
}

// Multiple deux matrices en place dans this (a * this)
void Matrix::mult_inv(Matrix *a) {
    if (this->ligne != a->col) {
        throw std::invalid_argument("lol");
    }

    double **newT = new double *[a->ligne];
    for (int i = 0; i < a->ligne; i++) {
        double *v = new double[this->col];
        for (int j = 0; j < this->col; j++) {
            v[j] = 0;
            for (int k = 0; k < this->ligne; k++) {
                v[j] += a->get(i, k) * this->get(k, j);
            }
        }
        newT[i] = v;
    }
	
	for(int i = 0; i < this->ligne; i++){
		free(this->t[i]);
	}

    free(this->t);
    this->t = newT;
    this->ligne = a->ligne;
}

void Matrix::add(Matrix *a) {
    if (this->col != a->col || this->ligne != a->ligne)
        throw std::invalid_argument("lol");

    for (int i = 0; i < this->col; i++) {
        for (int j = 0; j < this->ligne; j++) {
            this->set(i, j, this->get(i, j) + a->get(i, j));
        }
    }
}

void Matrix::mult_scal(double x) {
    for (int i = 0; i < this->col; i++) {
        for (int j = 0; j < this->ligne; j++) {
            this->set(i, j, this->get(i, j) * x);
        }
    }
}
