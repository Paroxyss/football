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

Chromosome::Chromosome() {
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
    std::cout << "Chromosome, " << EQUIPE_SIZE << " joueurs" << std::endl;
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        std::cout << "Joueur " << i << std::endl;
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->print();
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        this->didier[i]->print();
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

void Chromosome::apply(Matrix &inputs) {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        Matrix o = Matrix(NETWORK_INPUT_SIZE, 1);

        for (int j = 0; j < NETWORK_INPUT_SIZE; j++) {
            o.set(j, 0, inputs.get(j, i));
        }

        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            o.mult_inv(this->matrix[i][j]);
            apply_activation(o);
        }

        for (int j = 0; j < NETWORK_OUTPUT_SIZE; j++) {
            inputs.set(j, i, o.get(j, 0));
        }
    }
}

/*
    Didier est toujours évalué avant les joueurs. Lors de ce premier
    appel, inputs == NULL et il est alors évalués sur des 0.
    Sinon, inputs est une matrice colonne de taille COM_SIZE * EQUIPE_SIZE où
    les coefficients (inputs_i, ..., inputs_{i + COM_SIZE}) correspond aux
   sorties du canal de communication du joueur i lors de l'évaluation
   précédente.
*/

void Chromosome::apply_didier(Matrix &inputs) {
    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        if (inputs.ligne != this->didier[i]->col ||
            inputs.col != this->didier[i]->ligne) {

            // std::cout << "didier: " << this->didier[i]->ligne << " * "
            //           << this->didier[i]->col << std::endl;
        }
        inputs.mult_inv(this->didier[i]);
        apply_activation(inputs);
    }
}

/*
     Relecture très fortement nécessaire.
     lastcom est la matrice (COM_SIZE * EQUIPE_SIZE) contenant
     sur chaque colonnes les COM_SIZE dernières sorties de communication à
   Didier qui doivent être réinjectés dans compute_didier

   0 -> left
   1 -> right
*/

Matrix *Chromosome::collect_and_apply(player *p, ball *b, Matrix &didier_output,
                                      bool team) {
    Matrix *m = new Matrix(NETWORK_INPUT_SIZE, EQUIPE_SIZE);
    apply_didier(didier_output);

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        const player &selected = p[i];
        vector relat = p[i].pos - b[i].pos;
        double balldist = norme(relat);

        double fakeOrientation;
        vector fakePos;
        vector fakeVitesse = selected.vitesse;
        if (team) {
            // le joueur est du "mauvais coté" de la map, il faut adapter ses
            // entrées
            vector mapSize = {MAP_LENGTH, MAP_HEIGHT};
            fakeOrientation = PI - selected.orientation;
            fakePos = mapSize - selected.pos;
            fakeVitesse.x = -fakeVitesse.x;
        } else {
            fakeOrientation = selected.orientation;
            fakePos = selected.pos;
        }

        m->set(0, i, fakePos.x);
        m->set(1, i, fakePos.y);
        m->set(2, i, fakeVitesse.x);
        m->set(3, i, fakeVitesse.y);
        m->set(4, i, fakeOrientation);
        m->set(5, i, balldist);
        m->set(6, i, vangle(relat) - p->orientation);

        for (int j = 0; j < COM_SIZE; j++) {
            m->set(5 + j, i, didier_output.get(j + i * COM_SIZE, 0));
        }
    }

    // on prépare les lastcom suivant pour cette équipe
    this->apply(*m);

    for (int k = 0, c = 0; k < EQUIPE_SIZE; k++) {
        for (int i = 0; i < COM_SIZE; i++) {
            didier_output.set(c, 0,
                              m->get(NETWORK_OUTPUT_SIZE - COM_SIZE + i, k));
            c++;
        }
    }

    return m;
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
            delete nw->matrix[i][j];
            nw->matrix[i][j] = mutation(c->matrix[i][j]);
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        delete nw->didier[i];
        nw->didier[i] = mutation(c->didier[i]);
    }

    return nw;
}

Chromosome *crossover(Chromosome *a, Chromosome *b) {
    Chromosome *child = new Chromosome();

    for (int k = 0; k < EQUIPE_SIZE; k++) {
        for (int i = 0; i < NETWORK_SIZE - 1; i++) {

            Matrix *m = one_pointer_crossover(a->matrix[k][i], b->matrix[k][i]);

            for (int j = 0; j < m->ligne; j++) {
                for (int l = 0; l < m->col; l++) {
                    child->matrix[k][i]->set(j, l, m->get(j, l));
                }
            }

            delete m;
        }
    }

    for (int j = 0; j < DIDIER_NETWORK_SIZE - 1; j++) {
        Matrix *m = one_pointer_crossover(a->didier[j], b->didier[j]);
        for (int k = 0; k < m->ligne; k++) {
            for (int l = 0; l < m->col; l++) {
                child->didier[j]->set(k, l, m->get(k, l));
            }
        }
        delete m;
    }

    return child;
}
