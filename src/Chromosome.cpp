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

    std::cout << "didier:" << std::endl;
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

void Chromosome::initialize() {
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->initialize();
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        this->didier[i]->initialize();
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

Matrix *Chromosome::apply(Matrix &inputs) {
    Matrix *outputs = new Matrix(NETWORK_OUTPUT_SIZE, EQUIPE_SIZE);

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        Matrix o = Matrix(NETWORK_INPUT_SIZE, 1);

        for (int j = 0; j < NETWORK_INPUT_SIZE; j++) {
            o.set(j, 0, inputs.get(j, i));
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
    Didier est toujours évalué avant les joueurs. Lors de ce premier
    appel, inputs == NULL et il est alors évalués sur des 0.
    Sinon, inputs est une matrice colonne de taille COM_SIZE * EQUIPE_SIZE où
    les coefficients (inputs_i, ..., inputs_{i + COM_SIZE}) correspond aux
   sorties du canal de communication du joueur i lors de l'évaluation
   précédente.
*/

void Chromosome::apply_didier(Matrix &inputs) {
    inputs.mult_inv(*this->didier[0]);
    input_layer_activation(inputs);

    for (int i = 1; i < DIDIER_NETWORK_SIZE - 2; i++) {
        inputs.mult_inv(*this->didier[i]);
        hidden_layer_activation(inputs);
    }

    inputs.mult_inv(*this->didier[DIDIER_NETWORK_SIZE - 2]);
    output_layer_activation(inputs);
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
    mat->set(4, i, norme(b->pos - selected.pos));
    mat->set(
        5, i,
        angleRounded(vangle(b->pos - selected.pos) - selected.orientation));

    double fakeOrientation =
        team ? selected.orientation + M_PI : selected.orientation;
    // Distance et orientation relative de la cage adverse
    vector cage = {.x = MAP_LENGTH, .y = (double)MAP_HEIGHT / 2};
    mat->set(6, i, norme(cage - fakePos));
    mat->set(7, i, angleRounded(vangle(cage - fakePos) - fakeOrientation));

    // Joueur le plus proche
    player *nearest;
    double d = INFINITY;
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        double nd = norme(equipeAdverse[i].pos - selected.pos);
        if (nd < d) {
            nearest = &equipeAdverse[i];
            d = nd;
        };
    }
    mat->set(8, i, d);
    mat->set(9, i,
             angleRounded(vangle(nearest->pos - selected.pos) -
                          selected.orientation));
}

/*
   lastcom est la matrice (COM_SIZE * EQUIPE_SIZE) contenant
   sur chaque colonnes les COM_SIZE dernières sorties de communication à
   Didier qui doivent être réinjectés dans compute_didier

   team == false -> left
   team == true -> right
*/

Matrix *Chromosome::collect_and_apply(player *equipeAlliee,
                                      player *equipeAdverse, ball *b,
                                      Matrix &didier_output, bool team) {
    Matrix *inputs = new Matrix(NETWORK_INPUT_SIZE, EQUIPE_SIZE);
    apply_didier(didier_output);

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        writeInputs(inputs, equipeAlliee, equipeAdverse, i, b, team);

        // les coms
        for (int j = 0; j < COM_SIZE; j++) {
            inputs->set(NETWORK_INPUT_SIZE - COM_SIZE + j, i,
                        didier_output.get(j + i * COM_SIZE, 0));
        }
    }

    // Evaluation du réseau de neurones de chaque joueurs.
    Matrix *outputs = this->apply(*inputs);

    int c = 0;

    while (c < EQUIPE_SIZE) {
        for (int i = 0; i < COM_SIZE; i++) {
            didier_output.set(
                c * COM_SIZE + i, 0,
                outputs->get(NETWORK_OUTPUT_SIZE - COM_SIZE + i, c));
        }

        c++;
    }

    delete inputs;
    return outputs;
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
            nw->matrix[i][j] = mutation(*c->matrix[i][j]);
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        delete nw->didier[i];
        nw->didier[i] = mutation(*c->didier[i]);
    }

    return nw;
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

    for (int j = 0; j < DIDIER_NETWORK_SIZE - 1; j++) {
        Matrix *m = uniform_crossover(*a.didier[j], *b.didier[j]);
        for (int k = 0; k < m->ligne; k++) {
            for (int l = 0; l < m->col; l++) {
                child->didier[j]->set(k, l, m->get(k, l));
            }
        }
        delete m;
    }

    return child;
}

void Chromosome::write(std::ofstream &file) {
    int equipeSize = EQUIPE_SIZE;
    int nSize = NETWORK_SIZE;
    int dSize = DIDIER_NETWORK_SIZE;

    // Par sécurité
    file.write((char *)&equipeSize, sizeof(int));
    file.write((char *)&nSize, sizeof(int));
    file.write((char *)&dSize, sizeof(int));

    // Configs
    for (int i = 0; i < NETWORK_SIZE; i++) {
        file.write((char *)&PLAYER_LAYERS[i], sizeof(int));
    }
    for (int i = 0; i < DIDIER_NETWORK_SIZE; i++) {
        file.write((char *)&DIDIER_LAYERS[i], sizeof(int));
    }

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->write(file);
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        this->didier[i]->write(file);
    }
}

Chromosome *Chromosome::read(std::ifstream &file) {
    int equipeSize;
    int nSize;
    int dSize;

    // Par sécurité
    file.read((char *)&equipeSize, sizeof(int));
    file.read((char *)&nSize, sizeof(int));
    file.read((char *)&dSize, sizeof(int));

    if (equipeSize != EQUIPE_SIZE || nSize != NETWORK_SIZE ||
        dSize != DIDIER_NETWORK_SIZE) {
        throw std::invalid_argument(
            "Misconfigured Chromosome file (bad constants)");
    }

    const int pLayers[NETWORK_SIZE] = {};
    const int dLayers[DIDIER_NETWORK_SIZE] = {};

    // Configs
    for (int i = 0; i < NETWORK_SIZE; i++) {
        file.read((char *)&pLayers[i], sizeof(int));
        if (pLayers[i] != PLAYER_LAYERS[i]) {
            throw std::invalid_argument(
                "Misconfigured Chromosome file (bad player layers)");
        }
    }
    for (int i = 0; i < DIDIER_NETWORK_SIZE; i++) {
        file.read((char *)&dLayers[i], sizeof(int));
        if (dLayers[i] != DIDIER_LAYERS[i]) {
            throw std::invalid_argument(
                "Misconfigured Chromosome file (bad didier layers)");
        }
    }

    Chromosome *c = new Chromosome();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            delete c->matrix[i][j];
            c->matrix[i][j] = Matrix::read(file);
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        delete c->didier[i];
        c->didier[i] = Matrix::read(file);
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

double Chromosome::getDidierNorm() {
    double sum = 0;

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        for (int k = 0; k < didier[i]->ligne; k++) {
            for (int l = 0; l < didier[i]->col; l++) {
                sum += pow(didier[i]->get(k, l), 2);
            }
        }
    }

    return sqrt(sum);
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

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        for (int k = 0; k < didier[i]->ligne; k++) {
            for (int l = 0; l < didier[i]->col; l++) {
                sum += pow(didier[i]->get(k, l), 2);
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

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        for (int k = 0; k < didier[i]->ligne; k++) {
            for (int l = 0; l < didier[i]->col; l++) {
                v.x = didier[i]->get(k, l);
                sum += vangle(v);
            }
        }
    }

    return sum;
}

std::pair<double, double> Chromosome::get2dProjection() {
    return std::make_pair(this->getPlayersNorm(), this->getDidierNorm());
}
