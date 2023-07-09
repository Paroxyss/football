#include "Matrix.h"
#include "Mutation.hpp"
#include "config.h"
#include "util.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>

Matrix::Matrix(int ligne, int col) {
    this->ligne = ligne;
    this->col = col;
    this->t = new double[ligne * col];
}

Matrix::~Matrix() {
    delete[] this->t;
}

void Matrix::print() {
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
            double x = randomDouble(-10, 10);

            this->set(i, j, x);
        }
    }
}

/*
    Il existe énormément de différentes manières de
    muter une matrices, il faudrait faire des tests pour voir laquelle est la
   plus efficace
*/
Matrix *mutation(Matrix &m) {
    Matrix *muted = new Matrix(m.ligne, m.col);
    int np = m.ligne * m.col;
    int c = 0;

    for (int i = 0; i < m.ligne; i++) {
        for (int j = 0; j < m.col; j++) {
            if (likelyness(1 - GENE_MUTATION_PROBABILITY)) {
                continue;
            }

            muted->set(i, j, replacement());
        }
    }

    return muted;
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
            double v = m->get(i, j);
            file.read((char *)&v, sizeof(double));
        }
    }
    return m;
}
