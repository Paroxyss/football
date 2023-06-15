#include "Chromosome.hpp"
#include "Activation.hpp"
#include "Crossover.hpp"

#include "Game.hpp"
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
                new Matrix(PLAYER_LAYERS[j + 1], PLAYER_LAYERS[j]);
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        this->didier[i] = new Matrix(DIDIER_LAYERS[i + 1], DIDIER_LAYERS[i]);
    }
}

Chromosome::~Chromosome() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            delete this->matrix[i][j];
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        delete this->didier[i];
    }
}

void Chromosome::print() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->print();
        }
    }
}

/*
    Permet la génération de la population initiale.
    Les valeurs initialement générées sont pour le moment
    comprises entre -10 et 10.

    TODO: faut-il appliquer déjà ici la fonction d'activation
    coefficients?
*/

void Chromosome::randomize() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->randomize();
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        this->didier[i]->randomize();
    }
}

/*
    Code testé, ne pas toucher.

    inputs (resp. ouputs) est une matrice de taille NETWORK_INPUT_SIZE
   * EQUIPE_SIZE (resp. NETWORK_OUTPUT_SIZE * EQUIPE_SIZE) qui contient sur sa
   i-ème colonne les entrée (respectivement sortie) pour le i-ème joueur.

    Attention à faire attention c'est assez contre intuitif (il faudrait
   possiblement le changer plus tard) mais Chromosome possède bien sur sa i-ème
   LIGNE les (NETWORK_SIZE -1) matrices composant le NN du i-ème joueur
   tandis qu'ici les joueurs sont représentés sur les colonnes.

    A bien noter que chaque joueur possède autant de hidden layer et de même
   dimensions dans le bute d'éviter les "inégalités"
*/

Matrix *Chromosome::evaluate(Matrix *inputs) {
    Matrix *output = new Matrix(NETWORK_OUTPUT_SIZE, EQUIPE_SIZE);

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        Matrix *o = new Matrix(NETWORK_INPUT_SIZE, 1);

        for (int j = 0; j < NETWORK_INPUT_SIZE; j++) {
            o->set(j, 0, inputs->get(j, i));
        }

        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            o->mult_inv(this->matrix[i][j]);
            apply_activation(o);
        }

        for (int j = 0; j < NETWORK_OUTPUT_SIZE; j++) {
            output->set(j, i, o->get(j, 0));
        }
    }

    return output;
}

/*
    Didier est toujours évalué avant les joueurs. Lors de ce premier
    appel, inputs == NULL et il est alors évalués sur des 0.
    Sinon, inputs est une matrice colonne de taille COM_SIZE * EQUIPE_SIZE où
    les coefficients (inputs_i, ..., inputs_{i + COM_SIZE}) correspond aux
   sorties du canal de communication du joueur i lors de l'évaluation
   précédente.
*/

Matrix *Chromosome::compute_didier(Matrix *inputs) {
    if (!inputs) {
        Matrix *inputs = new Matrix(COM_SIZE * EQUIPE_SIZE, 1);
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        inputs->mult_inv(this->didier[i]);
        apply_activation(inputs);
    }

    return inputs;
}

/*
     Relecture très fortement nécessaire.
     lastcom est la matrice (COM_SIZE * EQUIPE_SIZE) contenant
     sur chaque colonnes les COM_SIZE dernières sorties de communication à
   Didier qui doivent être réinjectés dans compute_didier

*/

Matrix *Chromosome::collect_and_evaluate(player *p, Matrix *lastcom) {
    Matrix *m = new Matrix(NETWORK_INPUT_SIZE, EQUIPE_SIZE);
    Matrix *didier_output = compute_didier(lastcom);

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        m->set(0, i, p->pos.x);
        m->set(1, i, p->pos.y);
        m->set(2, i, p->vitesse.x);
        m->set(3, i, p->vitesse.y);
        m->set(4, i, p->orientation);
        for (int j = 0; j < COM_SIZE; j++) {
            m->set(5 + j, i, didier_output->get(j, i));
        }
    }

    // on prépare les lastcom suivant pour cette équipe
    Matrix *res = this->evaluate(m);
    if (!lastcom) {
        lastcom = new Matrix(COM_SIZE * EQUIPE_SIZE, 1);
    }

    for (int k = 0, c = 0; k < EQUIPE_SIZE; k++) {
        for (int i = 0; i < COM_SIZE; i++) {
            lastcom->set(c++, 0,
                         res->get(NETWORK_OUTPUT_SIZE - COM_SIZE + i, k));
        }
    }

    return res;
}

/*
    Pour mutter un chromosome (équipe) on mute pour le moment
    chaque matrice composant le chromosome.

    TODO: Une idée pourrait être également d'échanger la position de deux
   matrices (se qui échangerais la position initiale de 2 joueurs)
*/

Chromosome *mutate(Chromosome *c) {
    Chromosome *nw = new Chromosome();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            nw->matrix[i][j] = mutation(c->matrix[i][j]);
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        nw->didier[i] = mutation(c->didier[i]);
    }

    return nw;
}

Chromosome *crossover(Chromosome *a, Chromosome *b) {
    Chromosome *child = new Chromosome();

    for (int k = 0; k < EQUIPE_SIZE; k++) {
        for (int i = 0; i < NETWORK_SIZE - 1; i++) {
            Matrix *m = average_crossover(a->matrix[k][i], b->matrix[k][i]);

            for (int j = 0; j < m->ligne; j++) {
                for (int k = 0; k < m->col; k++) {
                    child->matrix[k][i]->set(j, k, m->get(j, k));
                }
            }
        }
    }

    for (int j = 0; j < DIDIER_NETWORK_SIZE - 1; j++) {
        Matrix *m = average_crossover(a->didier[j], b->didier[j]);
        for (int k = 0; k < m->ligne; k++) {
            for (int l = 0; l < m->col; l++) {
                child->didier[j]->set(k, l, m->get(k, l));
            }
        }
    }

    return child;
}
