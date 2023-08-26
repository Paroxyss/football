#include "Matrix.h"
#include "Mutation.hpp"
#include "config.h"
#include "util.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>

/**
 * @brief Créer une matrice.
 *
 * @param ligne Nombre de ligne de la matrice
 * @param col Nombre de colonnes de la matrice
 */
Matrix::Matrix(int ligne, int col) {
    this->ligne = ligne;
    this->col = col;
    this->t = new double[ligne * col];
}

/**
 * @brief Détruit la matrice après utilisation.
 */
Matrix::~Matrix() {
    delete[] this->t;
}

/**
 * @brief Affiche une matrice.
 */
void Matrix::print() {
    for (int i = 0; i < this->ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            std::cout << this->get(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

/**
 * @brief Permet d'initialiser les valeurs d'une matrice déjà instanciée.
 * Les moyennes sont initialisées à des valeurs comprises entre -1 et 1 et
 * de valeur moyenne 0.
 */
void Matrix::initialize() {
    for (int i = 0; i < this->ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            double x = randomDouble();

            this->set(i, j, x);
        }
    }
}

/**
 * @brief Permet d'accéder à une valeur de la matrice.
 *
 * @param i Ligne à laquelle on souhaite récupérer la valeur
 * @param j Colonne à laquelle on souhaite récupérer la valeur
 * @return Valeur de la matrice à la ligne `i` et colonne `j`
 */
double Matrix::get(int i, int j) {
    if (i >= this->ligne || j >= this->col) {
        throw std::invalid_argument("Bad matrice get");
    }

    return this->t[i * this->col + j];
}

/**
 * @brief Permet de changer une valeur de la matrice.
 *
 * @param i Ligne à laquelle changer la valeur
 * @param j Colonne à laquelle changer la valeur
 * @param x Valeur
 */
void Matrix::set(int i, int j, double x) {
    if (i >= this->ligne || j >= this->col) {
        throw std::invalid_argument("Bad matrice set");
    }

    this->t[i * this->col + j] = x;
}

/**
 * @brief Multiplie à gauche la matrice par une matrice passée en argument.
 * (a * this)
 *
 * @param a Matrice à multiplier à gauche
 */
void Matrix::mult_inv(Matrix &a) {
    if (this->ligne != a.col) {
        throw std::invalid_argument("lol multinv");
    }

    double *nt = new double[a.ligne * this->col];

    for (int i = 0; i < a.ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            nt[i * this->col + j] = 0;
            for (int k = 0; k < this->ligne; k++) {
                nt[i * this->col + j] += a.get(i, k) * this->get(k, j);
            }
        }
    }

    delete[] this->t;
    this->t = nt;
    this->ligne = a.ligne;
}

/**
 * @brief Mutation d'une matrice.
 * Il existe énormément de différentes manières de
 * muter une matrices, il faudrait faire des tests pour voir laquelle est la
 * plus efficace
 *
 *
 * @param m Matrice à muter.
 * @return Matrice passée en argument une fois mutée.
 */
void mutation(Matrix &m) {
    for (int i = 0; i < m.ligne; i++) {
        for (int j = 0; j < m.col; j++) {
            if (likelyness(1 - MUTATION_PROBABILITY)) {
                continue;
            }

            m.set(i, j, balance(m.get(i, j)));
        }
    }
}

/**
 * @brief Permet d'écrire une matrice dans un fichier.
 *
 * @param file Fichier dans lequel écrire la matrice.
 */
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

/**
 * @brief Permet de lire une matrice dans un fichier.
 *
 * @param file Fichier dans lequel lire la matrice.
 * @return La matrice lue.
 */
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
