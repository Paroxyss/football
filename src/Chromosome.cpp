#include "Chromosome.hpp"
#include "Activation.hpp"
#include "Crossover.hpp"

#include "Game.hpp"
#include "Matrix.h"
#include "Vector.hpp"
#include "config.h"
#include "util.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <ostream>

// Opérations couteuses donc mises en statiques, elles sont utilisées
// pour la normalisation min-max.
static double d_map = sqrt(pow(MAP_LENGTH, 2) + pow(MAP_HEIGHT, 2));
static double d_cage = sqrt(pow(MAP_HEIGHT / 2, 2) + pow(MAP_LENGTH, 2));

Chromosome::Chromosome() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j] =
                new Matrix(PLAYER_LAYERS[j + 1], PLAYER_LAYERS[j]);
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
    std::cout << "Chromosome, " << EQUIPE_SIZE << " joueurs" << std::endl;
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        std::cout << "Joueur " << i << std::endl;
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

void Chromosome::initialize() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->initialize();
        }
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

Matrix *Chromosome::apply(Matrix &inputs, player *equipeAlliee) {
    Matrix *outputs = new Matrix(NETWORK_OUTPUT_SIZE, EQUIPE_SIZE);

    for (int i = 0; i < EQUIPE_SIZE; i++) {

        // on peut retirer o puisque equipeAlliee[i] fait la même chose mais
        // j'ai pas envie de tout casser donc je te laisse le faire
        Matrix o = Matrix(NETWORK_INPUT_SIZE, 1);

        // on supprime les inputs du tick précédent.
        delete equipeAlliee[i].inputs;
        equipeAlliee[i].inputs = new Matrix(NETWORK_INPUT_SIZE, 1);

        for (int j = 0; j < NETWORK_INPUT_SIZE; j++) {
            o.set(j, 0, inputs.get(j, i));

            equipeAlliee[i].inputs->set(j, 0, inputs.get(j, i));
        }

        o.mult_inv(*this->matrix[i][0]);
        input_layer_activation(o);

        for (int j = 1; j < NETWORK_SIZE - 2; j++) {
            o.mult_inv(*this->matrix[i][j]);
            hidden_layer_activation(o);
        }

        o.mult_inv(*this->matrix[i][NETWORK_SIZE - 2]);
        output_layer_activation(o);

        for (int j = 0; j < NETWORK_OUTPUT_SIZE; j++) {
            outputs->set(j, i, o.get(j, 0));
        }
    }

    return outputs;
}

/*
    team == false -> gauche
    team == true -> droite
*/
void writeInputs(Matrix *mat, player *equipeAlliee, player *equipeAdverse,
                 int i, ball *b, bool team) {
    player &selected = equipeAlliee[i];

    // Position du joueur
    vector mapSize = {.x = MAP_LENGTH, .y = MAP_HEIGHT};
    vector fakePos = team ? mapSize - selected.pos : selected.pos;

    mat->set(0, i, fakePos.x);
    mat->set(1, i, fakePos.y);

    // Vitesse du joueur
    vector fakeVitesse = team ? -selected.vitesse : selected.vitesse;

    mat->set(2, i, fakeVitesse.x);
    mat->set(3, i, fakeVitesse.y);

    // Distance et orientation relative de la balle
    vector ball_fakePos = team ? mapSize - b->pos : b->pos;

    mat->set(4, i, norme(ball_fakePos - fakePos));
    mat->set(5, i, angleRounded(vangle(ball_fakePos - fakePos)));

    // Distance et orientation relative de la cage adverse
    vector cage = {.x = MAP_LENGTH, .y = (double)MAP_HEIGHT / 2};
    mat->set(6, i, norme(cage - fakePos));
    mat->set(7, i, angleRounded(vangle(cage - fakePos)));

    // Joueur le plus proche
    vector nearest;
    double d = INFINITY;

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        vector opponent_fakePos =
            team ? mapSize - equipeAdverse[i].pos : equipeAdverse[i].pos;

        double nd = norme(opponent_fakePos - fakePos);
        if (nd < d) {
            nearest = opponent_fakePos;
            d = nd;
        };
    }

    mat->set(8, i, d);
    mat->set(9, i, angleRounded(vangle(nearest - fakePos)));
}

Matrix *Chromosome::collect_and_apply(player *equipeAlliee,
                                      player *equipeAdverse, ball *b,
                                      bool team) {
    Matrix *inputs = new Matrix(NETWORK_INPUT_SIZE, EQUIPE_SIZE);

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        writeInputs(inputs, equipeAlliee, equipeAdverse, i, b, team);
    }

    // Evaluation du réseau de neurones de chaque joueurs.
    Matrix *outputs = this->apply(*inputs, equipeAlliee);

    int c = 0;

    delete inputs;
    return outputs;
}

/*
    Pour mutter un chromosome (équipe) on mute pour le moment
    chaque matrice composant le chromosome.

    TODO: Une idée pourrait être également d'échanger la position de deux
   matrices (se qui échangerais la position initiale de 2 joueurs)
*/

void mutate(Chromosome &c) {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            mutation(*c.matrix[i][j]);
        }
    }
}

Chromosome *crossover(Chromosome &a, Chromosome &b) {
    Chromosome *child = new Chromosome();

    for (int k = 0; k < EQUIPE_SIZE; k++) {
        for (int i = 0; i < NETWORK_SIZE - 1; i++) {

            Matrix *m = uniform_crossover(*a.matrix[k][i], *b.matrix[k][i]);

            for (int j = 0; j < m->ligne; j++) {
                for (int l = 0; l < m->col; l++) {
                    child->matrix[k][i]->set(j, l, m->get(j, l));
                }
            }

            delete m;
        }
    }

    return child;
}

void Chromosome::write(std::ofstream &file) {
    int equipeSize = EQUIPE_SIZE;
    int nSize = NETWORK_SIZE;

    // Par sécurité
    file.write((char *)&equipeSize, sizeof(int));
    file.write((char *)&nSize, sizeof(int));

    // Configs
    for (int i = 0; i < NETWORK_SIZE; i++) {
        file.write((char *)&PLAYER_LAYERS[i], sizeof(int));
    }

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->write(file);
        }
    }
}

Chromosome *Chromosome::read(std::ifstream &file) {
    int equipeSize;
    int nSize;

    // Par sécurité
    file.read((char *)&equipeSize, sizeof(int));
    file.read((char *)&nSize, sizeof(int));

    if (equipeSize != EQUIPE_SIZE || nSize != NETWORK_SIZE) {
        throw std::invalid_argument(
            "Misconfigured Chromosome file (bad constants)");
    }

    const int pLayers[NETWORK_SIZE] = {};
	
    // Configs
    for (int i = 0; i < NETWORK_SIZE; i++) {
        file.read((char *)&pLayers[i], sizeof(int));
        if (pLayers[i] != PLAYER_LAYERS[i]) {
            throw std::invalid_argument(
                "Misconfigured Chromosome file (bad player layers)");
        }
    }
	
    Chromosome *c = new Chromosome();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            delete c->matrix[i][j];
            c->matrix[i][j] = Matrix::read(file);
        }
    }

    return c;
}

double Chromosome::getPlayersNorm() {
    double sum = 0;

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            for (int k = 0; k < matrix[i][j]->ligne; k++) {
                for (int l = 0; l < matrix[i][j]->col; l++) {
                    sum += pow(matrix[i][j]->get(k, l), 2);
                }
            }
        }
    }
    return sum;
}

double Chromosome::getMatrixesNorm() {
    double sum = 0;

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            for (int k = 0; k < matrix[i][j]->ligne; k++) {
                for (int l = 0; l < matrix[i][j]->col; l++) {
                    sum += pow(matrix[i][j]->get(k, l), 2);
                }
            }
        }
    }

    return sqrt(sum);
}

double Chromosome::getAngleNorm() {
    double sum = 0;
    vector v = {0, 1};

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            for (int k = 0; k < matrix[i][j]->ligne; k++) {
                for (int l = 0; l < matrix[i][j]->col; l++) {
                    v.x = matrix[i][j]->get(k, l);
                    sum += vangle(v);
                }
                return sum;
            }
        }
    }
    return sum;
}

