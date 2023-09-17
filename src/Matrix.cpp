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
    if (ligne * col <= DEFAULT_M_SIZE) {
        this->t = new double[DEFAULT_M_SIZE];
        this->ct = new double[DEFAULT_M_SIZE];
    }
}

/**
 * @brief Détruit la matrice après utilisation.
 */
Matrix::~Matrix() {
    delete[] this->t;
    delete[] this->ct;
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
    STRICT_ASSERT(i >= this->ligne || j >= this->col, "Bad matrice get")
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
    STRICT_ASSERT(i >= this->ligne || j >= this->col, "Bad matrice set")
    this->t[i * this->col + j] = x;
}

/**
 * @brief Multiplie à gauche la matrice par une matrice passée en argument.
 * (a * this)
 *
 * @param a Matrice à multiplier à gauche
 */
#define STATICMULT(l, c)                                                       \
    case (l + (c << 8)):                                                       \
        for (int i = 0; i < l; i++) {                                          \
            for (int j = 0; j < c; j++) {                                      \
                this->ct[i * c + j] = 0;                                       \
                for (int k = 0; k < l; k++) {                                  \
                    double v1 = this->t[i * c + k];                            \
                    double v2 = a.t[k * c + j];                                \
                    this->ct[i * c + j] += v1 * v2;                            \
                }                                                              \
            }                                                                  \
        }                                                                      \
        break

void Matrix::mult_inv(Matrix &a) {
    STRICT_ASSERT(this->ligne != a.col, "lol multinv")
    STRICT_ASSERT(a.ligne * this->col > DEFAULT_M_SIZE,
                  "Multiv avec des matrices trop grandes")

    switch (a.ligne + (this->col << 8)) {
        STATICMULT(12, 1);
        STATICMULT(10, 1);
        STATICMULT(8, 1);
        STATICMULT(6, 1);
        STATICMULT(4, 1);
        STATICMULT(2, 1);
    default:
        std::cout << "Attention: calcul de matrice non statique : " << a.ligne
                  << ", " << this->col << std::endl;
        lent_mult_inv(a);
        return;
    }

    auto tmp = this->ct;
    this->ct = this->t;
    this->t = tmp;

    this->ligne = a.ligne;
}

void Matrix::lent_mult_inv(Matrix &a) {
    if (this->ligne != a.col) {
        throw std::invalid_argument("lol multinv");
    }
    if (a.ligne * this->col > DEFAULT_M_SIZE) {
        std::cout << a.ligne << "x" << this->col << std::endl;
        throw std::invalid_argument("Multiv avec des matrices trop grandes");
    }

    for (int i = 0; i < a.ligne; i++) {
        for (int j = 0; j < this->col; j++) {
            this->ct[i * this->col + j] = 0;
            for (int k = 0; k < this->ligne; k++) {
                this->ct[i * this->col + j] += a.get(i, k) * this->get(k, j);
            }
        }
    }

    auto tmp = this->ct;
    this->ct = this->t;
    this->t = tmp;

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

            m.set(i, j, replacement());
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
