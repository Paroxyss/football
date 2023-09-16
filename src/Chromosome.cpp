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

Matrix *Chromosome::apply(player *equipeAlliee) {
    Matrix *outputs = new Matrix(NETWORK_OUTPUT_SIZE, EQUIPE_SIZE);

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        player &selected = equipeAlliee[i];

        // Cette matrice va servir à faire le calcul en place
        Matrix mcalcul = Matrix(NETWORK_INPUT_SIZE, 1);

        // On copie la matrice d'input dans celle de calcul
        for (int j = 0; j < NETWORK_INPUT_SIZE; j++) {
            mcalcul.set(j, 0, selected.inputs->get(j, 0));
        }

        mcalcul.mult_inv(*this->matrix[i][0]);
        input_layer_activation(mcalcul);

        for (int j = 1; j < NETWORK_SIZE - 2; j++) {
            mcalcul.mult_inv(*this->matrix[i][j]);
            hidden_layer_activation(mcalcul);
        }

        mcalcul.mult_inv(*this->matrix[i][NETWORK_SIZE - 2]);
        output_layer_activation(mcalcul);

        for (int j = 0; j < NETWORK_OUTPUT_SIZE; j++) {
            outputs->set(j, i, mcalcul.get(j, 0));
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

/*void Chromosome::apply_didier(Matrix &inputs) {
    inputs.mult_inv(*this->didier[0]);
    input_layer_activation(inputs);

    for (int i = 1; i < DIDIER_NETWORK_SIZE - 2; i++) {
        inputs.mult_inv(*this->didier[i]);
        hidden_layer_activation(inputs);
    }

    inputs.mult_inv(*this->didier[DIDIER_NETWORK_SIZE - 2]);
    output_layer_activation(inputs);
}*/

/**
 * @brief min-max normalization: Retranche la valeur donnée entre 0 et 1.
 */
double mmn(double x, double min, double max) {
    return (x - min) / (max - min);
}
double mmn_negatif(double x, double min, double max) {
    return mmn(x, min, max) * 2 - 1;
}
double normalizeAngle(double a) {
    return mmn_negatif(a, -M_PI, M_PI);
}

/**
 * @brief Si les valeurs d'entrées ne sont pas normalisées, certaines valeurs
 * qui sont naturellement plus élevée, comme par exemple la distance à la cage
 * par rapport à la distance à la balle seront plus importantes que d'autres qui
 * peuvent être naturellement plus petite comme l'orientation.
 */
void normalize_inputs(Matrix &inputs) {
    // Position du joueur
    inputs.set(0, 0,
               mmn(inputs.get(0, 0), PLAYER_SIZE, MAP_LENGTH - PLAYER_SIZE));
    inputs.set(1, 0,
               mmn(inputs.get(1, 0), PLAYER_SIZE, MAP_HEIGHT - PLAYER_SIZE));

    // Vitesse du joueur
    double vmax = PLAYER_ACCELERATION / PLAYER_FROTTEMENT;

    inputs.set(2, 0, mmn(inputs.get(2, 0), 0, vmax));
    inputs.set(3, 0, normalizeAngle(inputs.get(3, 0)));

    // Distance et orientation relative de la balle
    inputs.set(4, 0,
               mmn(inputs.get(4, 0), BALL_SIZE + PLAYER_SIZE,
                   d_map - BALL_SIZE - PLAYER_SIZE));
    inputs.set(5, 0, normalizeAngle(inputs.get(5, 0)));

    // Distance et orientation relative de la cage adverse
    inputs.set(6, 0, mmn(inputs.get(6, 0), PLAYER_SIZE, d_cage - PLAYER_SIZE));
    inputs.set(7, 0, normalizeAngle(inputs.get(7, 0)));

    // Joueur le plus proche
    inputs.set(8, 0,
               mmn(inputs.get(8, 0), 2 * PLAYER_SIZE, d_map - 2 * PLAYER_SIZE));
    inputs.set(9, 0, normalizeAngle(inputs.get(9, 0)));

    // TODO: Il faut trouver un moyen de normaliser didier...
    // TODO: Il faut trouver un moyen de passer sur le fichier game.csv les
    // valeurs non normalisées.
}

/*
    team == false -> gauche
    team == true -> droite
*/
void writeInputs(player &target, player *equipeAdverse, ball *b, bool team) {
    Matrix *&mat = target.inputs;

    // Position du joueur
    vector mapSize = {.x = MAP_LENGTH, .y = MAP_HEIGHT};
    vector fakePos = team ? mapSize - target.pos : target.pos;

    mat->set(0, 0, fakePos.x);
    mat->set(1, 0, fakePos.y);

    mat->set(2, 0, norme(target.vitesse));
    mat->set(3, 0, -angleRounded(target.orientation - vangle(target.vitesse)));

    mat->set(4, 0, norme(b->pos - target.pos));
    // on l'inverse pour que ce soit la valeur à corriger pour aller vers la
    // balle, plus logique pour le joueur
    mat->set(5, 0,
             -angleRounded(target.orientation - vangle(b->pos - target.pos)));

    // Distance et orientation relative de la cage adverse
    vector cage = {.x = static_cast<double>(team ? 0 : MAP_LENGTH),
                   .y = (double)MAP_HEIGHT / 2};
    mat->set(6, 0, norme(cage - target.pos));
    // idem que pour l'orientation relative de la balle, on passe la valeur
    // angulaire à corriger
    mat->set(7, 0,
             -angleRounded(target.orientation - vangle(cage - target.pos)));

    // Joueur le plus proche
    vector nearest;
    double d = INFINITY;

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        double nd = norme(equipeAdverse[i].pos - target.pos);
        if (nd < d) {
            nearest = equipeAdverse[i].pos;
            d = nd;
        };
    }

    mat->set(8, 0, d);
    mat->set(9, 0,
             -angleRounded(target.orientation - vangle(nearest - target.pos)));

    normalize_inputs(*mat);
}

Matrix *Chromosome::collect_and_apply(player *equipeAlliee,
                                      player *equipeAdverse, ball *b,
                                      bool team) {
    // On sauvegarde les inputs dans les joueurs
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        writeInputs(equipeAlliee[i], equipeAdverse, b, team);
    }

    // Evaluation du réseau de neurones de chaque joueurs.
    Matrix *outputs = this->apply(equipeAlliee);

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

    // Stats
    file.write((char *)&this->stats.instanceAge, sizeof(int));
    file.write((char *)&this->stats.instanceGoals, sizeof(int));

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
            std::cout << i << ":" << pLayers[i] << " =/= " << PLAYER_LAYERS[i];
            throw std::invalid_argument(
                "Misconfigured Chromosome file (bad player layers)");
        }
    }

    Chromosome *c = new Chromosome();

    // Stats
    file.read((char *)&c->stats.instanceAge, sizeof(int));
    file.read((char *)&c->stats.instanceGoals, sizeof(int));

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
