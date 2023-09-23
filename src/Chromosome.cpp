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
static const double d_map = sqrt(pow(MAP_LENGTH, 2) + pow(MAP_HEIGHT, 2));
static const double d_bordure_cage =
    sqrt(pow((MAP_HEIGHT + GOAL_HEIGHT) / 2, 2) + pow(MAP_LENGTH, 2));

// Vitesse maximum du joueur
static const double vjmax = PLAYER_ACCELERATION / PLAYER_FROTTEMENT;
// Vitesse maximum de la balle
static const double vbmax = vjmax * sqrt(PLAYER_MASS / BALL_MASS);

Chromosome::Chromosome() {
    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        this->didier[i] = new Matrix(DIDIER_LAYERS[i + 1], DIDIER_LAYERS[i]);
    }

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
}

/*
    Permet la génération de la population initiale.
    Les valeurs initialement générées sont pour le moment
    comprises entre -10 et 10.

    TODO: faut-il appliquer déjà ici la fonction d'activation
    coefficients?
*/

void Chromosome::initialize() {
    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        this->didier[i]->initialize();
    }

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            this->matrix[i][j]->initialize();
        }
    }
}

void Chromosome::apply_didier(Matrix &inputs) {
    Matrix mcalcul = Matrix(EQUIPE_SIZE * COM_SIZE, 1);

    for (int i = 0; i < EQUIPE_SIZE * COM_SIZE; i++) {
        mcalcul.set(i, 0, inputs.get(i, 0));
    }

    mcalcul.mult_inv(*this->didier[0]);
    input_layer_activation(mcalcul);

    for (int j = 1; j < DIDIER_NETWORK_SIZE - 2; j++) {
        mcalcul.mult_inv(*this->didier[j]);
        hidden_layer_activation(inputs);
    }

    mcalcul.mult_inv(*this->didier[DIDIER_NETWORK_SIZE - 2]);
    output_layer_activation(mcalcul);

    for (int j = 0; j < COM_SIZE * EQUIPE_SIZE; j++) {
        inputs.set(j, 0, mcalcul.get(j, 0));
    }
}

/*
    Code testé, ne pas toucher.

    inputs (resp. ouputs) est une matrice de taille NETWORK_INPUT_SIZE
   * EQUIPE_SIZE (resp. NETWORK_OUTPUT_SIZE * EQUIPE_SIZE) qui contient sur
   sa i-ème colonne les entrée (respectivement sortie) pour le i-ème joueur.

    Attention à faire attention c'est assez contre intuitif (il faudrait
   possiblement le changer plus tard) mais Chromosome possède bien sur sa
   i-ème LIGNE les (NETWORK_SIZE -1) matrices composant le NN du i-ème
   joueur tandis qu'ici les joueurs sont représentés sur les colonnes.

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
    Sinon, inputs est une matrice colonne de taille COM_SIZE * EQUIPE_SIZE
   où les coefficients (inputs_i, ..., inputs_{i + COM_SIZE}) correspond aux
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
 * @brief Si les valeurs d'entrées ne sont pas normalisées, certaines
 * valeurs qui sont naturellement plus élevée, comme par exemple la distance
 * à la cage par rapport à la distance à la balle seront plus importantes
 * que d'autres qui peuvent être naturellement plus petite comme
 * l'orientation.
 */
void normalize_inputs(Matrix &inputs) {

    // Vitesse du joueur
    inputs.set(0, 0, mmn(inputs.get(0, 0), 0, vjmax));
    inputs.set(1, 0, normalizeAngle(inputs.get(1, 0)));

    // Distance et orientation relative des limites la cage adverse
    // haut
    inputs.set(
        2, 0, mmn(inputs.get(2, 0), PLAYER_SIZE, d_bordure_cage - PLAYER_SIZE));
    inputs.set(3, 0, normalizeAngle(inputs.get(3, 0)));
    // bas
    inputs.set(
        4, 0, mmn(inputs.get(4, 0), PLAYER_SIZE, d_bordure_cage - PLAYER_SIZE));
    inputs.set(5, 0, normalizeAngle(inputs.get(5, 0)));

    // Distance et orientation relative de la balle
    inputs.set(6, 0,
               mmn(inputs.get(6, 0), BALL_SIZE + PLAYER_SIZE,
                   d_map - BALL_SIZE - PLAYER_SIZE));
    inputs.set(7, 0, normalizeAngle(inputs.get(7, 0)));
    inputs.set(8, 0, mmn(inputs.get(8, 0), 0, vjmax + vbmax));
    inputs.set(9, 0, normalizeAngle(inputs.get(9, 0)));

    // Joueur ami le plus proche
    inputs.set(
        10, 0,
        mmn(inputs.get(10, 0), 2 * PLAYER_SIZE, d_map - 2 * PLAYER_SIZE));
    inputs.set(11, 0, normalizeAngle(inputs.get(11, 0)));
    inputs.set(12, 0, mmn(inputs.get(12, 0), 0, 2 * vjmax));
    inputs.set(13, 0, normalizeAngle(inputs.get(13, 0)));

    // Joueur adverse le plus proche
    inputs.set(
        14, 0,
        mmn(inputs.get(14, 0), 2 * PLAYER_SIZE, d_map - 2 * PLAYER_SIZE));
    inputs.set(15, 0, normalizeAngle(inputs.get(15, 0)));
    inputs.set(16, 0, mmn(inputs.get(16, 0), 0, 2 * vjmax));
    inputs.set(17, 0, normalizeAngle(inputs.get(17, 0)));

    // TODO: Il faut trouver un moyen de normaliser didier...
    // TODO: Il faut trouver un moyen de passer sur le fichier game.csv les
    // valeurs non normalisées.
}

/*
    team == false -> gauche
    team == true -> droite
*/
void writeInputs(player &target, player *equipeAlliee, player *equipeAdverse,
                 ball *b, bool team) {
    Matrix *&mat = target.inputs;

    mat->set(0, 0, norme(target.vitesse));
    mat->set(1, 0, -angleRounded(target.orientation - vangle(target.vitesse)));

    vector centreCage = {.x = static_cast<double>(team ? 0 : MAP_LENGTH),
                         .y = (double)MAP_HEIGHT / 2};

    vector cageHaut = {.x = centreCage.x,
                       .y = centreCage.y + (double)GOAL_HEIGHT / 2.};
    auto normeC1 = norme(cageHaut - target.pos);
    auto angleC1 =
        -angleRounded(target.orientation - vangle(cageHaut - target.pos));
    ;

    vector cageBas = {.x = centreCage.x,
                      .y = centreCage.y - (double)GOAL_HEIGHT / 2.};
    auto normeC2 = norme(cageBas - target.pos);
    auto angleC2 =
        -angleRounded(target.orientation - vangle(cageBas - target.pos));
    ;

    // Position des cages
    // idem que pour l'orientation relative de la balle, on passe la valeur
    // angulaire à corriger
    if (team) {
        mat->set(2, 0, normeC1);
        mat->set(3, 0, angleC1);
        mat->set(4, 0, normeC2);
        mat->set(5, 0, angleC2);
    } else {
        mat->set(2, 0, normeC2);
        mat->set(3, 0, angleC2);
        mat->set(4, 0, normeC1);
        mat->set(5, 0, angleC1);
    }

    // Position de la balle
    mat->set(6, 0, norme(b->pos - target.pos));
    // on l'inverse pour que ce soit la valeur à corriger pour aller vers la
    // balle, plus logique pour le joueur
    // Distance et orientation relative de la cage adverse
    mat->set(7, 0,
             -angleRounded(target.orientation - vangle(b->pos - target.pos)));

    vector vitesseRelatBalle = b->vitesse - target.vitesse;

    mat->set(8, 0, norme(vitesseRelatBalle));
    mat->set(9, 0,
             -angleRounded(target.orientation - vangle(vitesseRelatBalle)));

    // Joueur le plus proche
    player *nearestCopain = &target;
    double d = INFINITY;

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        double nd = norme(equipeAlliee[i].pos - target.pos);
        if (nd < d && nd > PLAYER_SIZE / 2.) {
            nearestCopain = &equipeAlliee[i];
            d = nd;
        };
    }

    mat->set(10, 0, d);
    mat->set(11, 0,
             -angleRounded(target.orientation -
                           vangle(nearestCopain->pos - target.pos)));

    vector vitesseRelatCopain = nearestCopain->vitesse - target.vitesse;

    // pour éviter des mauvais atan au début
    auto vCopain = norme(vitesseRelatCopain);
    mat->set(12, 0, vCopain);
    if (vCopain == 0) {
        mat->set(13, 0, 0);
    } else {
        mat->set(
            13, 0,
            -angleRounded(target.orientation - vangle(vitesseRelatCopain)));
    }

    // Joueur le plus proche
    player *nearestAdv;
    d = INFINITY;

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        double nd = norme(equipeAdverse[i].pos - target.pos);
        if (nd < d) {
            nearestAdv = &equipeAdverse[i];
            d = nd;
        };
    }

    mat->set(14, 0, d);
    mat->set(15, 0,
             -angleRounded(target.orientation -
                           vangle(nearestAdv->pos - target.pos)));

    vector vitesseRelatadv = nearestAdv->vitesse - target.vitesse;

    mat->set(16, 0, norme(vitesseRelatadv));
    mat->set(17, 0,
             -angleRounded(target.orientation - vangle(vitesseRelatadv)));

    normalize_inputs(*mat);
}

Matrix *Chromosome::collect_and_apply(player *equipeAlliee,
                                      player *equipeAdverse,
                                      Matrix &didier_output, ball *b,
                                      bool team) {
    // On applique didier
    apply_didier(didier_output);

    // On sauvegarde les inputs dans les joueurs
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        writeInputs(equipeAlliee[i], equipeAlliee, equipeAdverse, b, team);

        // Pour l'instant didier est injecté ici j'avais la flemme de
        // changer la signature de writeInputs
        for (int j = 0; j < COM_SIZE; j++) {

            equipeAlliee[i].inputs->set(NETWORK_INPUT_SIZE - COM_SIZE + j, 0,
                                        didier_output.get(i * COM_SIZE + j, 0));
        }
    }

    // Evaluation du réseau de neurones de chaque joueurs.
    Matrix *outputs = this->apply(equipeAlliee);

    // Mise à jour de didier pour le tick suivant.
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < COM_SIZE; j++) {
            didier_output.set(
                i * COM_SIZE + j, 0,
                outputs->get(NETWORK_OUTPUT_SIZE - COM_SIZE + j, i));
        }
    }

    return outputs;
}

/*
    Pour mutter un chromosome (équipe) on mute pour le moment
    chaque matrice composant le chromosome.

    TODO: Une idée pourrait être également d'échanger la position de deux
   matrices (se qui échangerais la position initiale de 2 joueurs)
*/

void mutate(Chromosome &c) {
    // on mute le chromosome donc il perd en capacité, donc ses buts
    // précédents doivent être moins prépondérants
    c.stats.instanceGoals = (double)c.stats.instanceGoals / 2.;
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            mutation(*c.matrix[i][j]);
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        mutation(*c.didier[i]);
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

    // Stats
    file.write((char *)&this->stats.instanceAge, sizeof(int));
    file.write((char *)&this->stats.instanceGoals, sizeof(int));

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
            std::cout << i << ":" << pLayers[i] << " =/= " << PLAYER_LAYERS[i];
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

    // Stats
    file.read((char *)&c->stats.instanceAge, sizeof(int));
    file.read((char *)&c->stats.instanceGoals, sizeof(int));

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
