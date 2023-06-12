#include "Chromosome.hpp"
#include "Matrix.h"
#include "config.h"
#include <array>
#include <cmath>
#include <iostream>
#include <ostream>

void Chromosome::initialize() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j] =
                new Matrix(LAYER_CONFIG[j + 1], LAYER_CONFIG[j]);
        }
    }
}

Chromosome::~Chromosome() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            delete this->matrix[i][j];
        }
    }
}

void Chromosome::print() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->print();
        }
    }
}

void Chromosome::randomize() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->randomize();
        }
    }
}

Chromosome *mutate(Chromosome *c) {
    Chromosome *nw = new Chromosome();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            nw->matrix[i][j] = mutation(c->matrix[i][j]);
        }
    }

    return nw;
}

/*

    Code testé, ne pas toucher.

    inputs (respectivement ouputs) est une matrice de taille NETWORK_INPUT_SIZE * EQUIPE_SIZE qui
    contient sur sa i-ème colonne les entrée (respectivement sortie) pour le i-ème joueur. 

    Attention à faire attention c'est assez contre intuitif (il faudrait possiblement le changer plus tard)
    mais Chromosome possède bien sur sa i-ème LIGNE les (NETWORK_SIZE -1) matrices composant sont réseau
    tandis qu'ici les joueurs sont représentés sur les colonnes.

    A bien noter que chaque joueur possède autant de hidden layer et de même dimensions
    dans le bute d'éviter les "inégalités"
*/

Matrix *evaluate(Matrix *inputs, Chromosome *c) {
    Matrix *output = new Matrix(NETWORK_OUTPUT_SIZE, EQUIPE_SIZE);

    for(int i = 0; i < EQUIPE_SIZE; i++) {
        Matrix *o = new Matrix(NETWORK_INPUT_SIZE, 1);

        for(int j = 0; j < NETWORK_INPUT_SIZE; j++)  {
            o->set(j, 0, inputs->get(j, i));
        }

        for(int j = 0; j < NETWORK_SIZE - 1; j++) {
            o->mult_inv(c->matrix[i][j]);

            for(int k = 0; k < o->ligne; k++) {
                for(int v = 0; v < o->col; v++) {
                    o->set(k, v, tanh(o->get(k, v)));
                }
            }
        }

        for(int j = 0; j < NETWORK_OUTPUT_SIZE; j++) {
            output->set(j, i, o->get(j, 0));
        }
    }

    return output;
}
