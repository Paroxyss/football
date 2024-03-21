#include "Chromosome.hpp"
#include "Activation.hpp"
#include "Crossover.hpp"
#include "Game.hpp"
#include "Genealogy.hpp"
#include "Inputs.hpp"
#include "Matrix.h"
#include "Vector.hpp"
#include "config.h"
#include "util.hpp"

#include <cmath>
#include <iostream>
#include <ostream>
#include <stdexcept>

Chromosome::Chromosome() {
    this->id = uid();
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
    std::cout << "Didier: " << std::endl;
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
	this->hasDidier = likelyness(0.5);
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
   dimensions dans le but d'éviter les "inégalités"
*/

void Chromosome::apply(player *equipeAlliee) {
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
            selected.outputs->set(j, 0, mcalcul.get(j, 0));
        }
    }
}

/*
    Didier est toujours évalué avant les joueurs.
    Chaque joueur contient les outputs qu'il a donné à la fin du tick précédent.
   On les passe en entrée pour didier, en sortie, didier écrit lets inputs
   destinés aux joueurs dans leurs champ output
*/

void Chromosome::apply_didier(player *equipeAlliee) {
    Matrix inputs = Matrix(COM_SIZE * EQUIPE_SIZE, 1);

    // std::cout << "Didier reçoit pour le joueur 1 :
    // ";equipeAlliee[0].outputs->print();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < COM_SIZE; j++) {
            inputs.set(i * COM_SIZE + j, 0,
                       equipeAlliee[i].outputs->get(
                           NETWORK_OUTPUT_SIZE - COM_SIZE + j, 0));
        }
    }

    // std::cout << "INputs remplis:" << std::endl;
    // inputs.print();
    // inputs.mult_inv(*this->didier[0]);
    // std::cout << "firstLayer" << std::endl;
    // inputs.print();
    // input_layer_activation(inputs);
    /*std::cout << "INPUTS DIDIER" << std::endl;
    inputs.print();
    std::cout << "MULTIPLICATION AVEC CE RÉSEAU:" << std::endl;
    this->didier[0]->print();
*/
    for (int i = 1; i < DIDIER_NETWORK_SIZE - 2; i++) {
        inputs.mult_inv(*this->didier[i]);
        // std::cout << "layer "<< i << std::endl;
        // inputs.print();
        hidden_layer_activation(inputs);
        // std::cout << "hidden" << std::endl;
        // inputs.print();
    }

    inputs.mult_inv(*this->didier[DIDIER_NETWORK_SIZE - 2]);
    output_layer_activation(inputs);
    /*std::cout << "OUTPUT DIDIER" << std::endl;
    inputs.print();

*/
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < COM_SIZE; j++) {
            equipeAlliee[i].inputs->set(j, 0, inputs.get(i * COM_SIZE + j, 0));
        }
    }
}

void Chromosome::collect_and_apply(player *equipeAlliee, player *equipeAdverse,
                                   ball *b, bool team) {
    if (this->hasDidier) {
        apply_didier(equipeAlliee);
    } else {
        for (int i = 0; i < EQUIPE_SIZE; i++) {
            for (int j = 0; j < COM_SIZE; j++) {
                equipeAlliee[i].inputs->set(j, 0, 0);
            }
        }
    }
    // On sauvegarde les inputs dans les joueurs
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        writeInputs(equipeAlliee[i], equipeAlliee, equipeAdverse, b, team);
    }

    // Evaluation du réseau de neurones de chaque joueurs.
    this->apply(equipeAlliee);
}

/*
    Pour mutter un chromosome (équipe) on mute pour le moment
    chaque matrice composant le chromosome.

    TODO: Une idée pourrait être également d'échanger la position de deux
   matrices (se qui échangerais la position initiale de 2 joueurs)
*/

void mutate(Chromosome &c) {
    // on mute le chromosome donc il perd en capacité, donc ses buts précédents
    // doivent être moins prépondérants
    c.stats.instanceGoals = (double)c.stats.instanceGoals / 2.;
    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            mutation(*c.matrix[i][j]);
        }
    }

    if (likelyness(SWAP_MUTATION_PROBA)) {
        int a = thrand(0, EQUIPE_SIZE - 1);
        int b = thrand(0, EQUIPE_SIZE - 2);
        if (a == b)
            b++;

        for (int i = 0; i < NETWORK_SIZE - 1; i++) {
            for (int j = 0; j < c.matrix[a][i]->col; j++) {
                for (int k = 0; k < c.matrix[a][i]->ligne; k++) {
                    int tmp = c.matrix[a][i]->get(k, j);
                    c.matrix[a][i]->set(k, j, c.matrix[b][i]->get(k, j));
                    c.matrix[b][i]->set(k, j, tmp);
                }
            }
        }
    }
    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        mutation(*c.didier[i]);
    }
}

Chromosome *classicCrossover(Chromosome &a, Chromosome &b) {
    Chromosome *child = new Chromosome();
    child->hasDidier = likelyness(0.5 * (a.hasDidier + b.hasDidier));

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

    int nDidier = a.hasDidier + b.hasDidier;

    switch (nDidier) {
    case 0:
        break;
    case 2:
        for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
            Matrix *m = uniform_crossover(*a.didier[i], *b.didier[i]);

            for (int j = 0; j < m->ligne; j++) {
                for (int l = 0; l < m->col; l++) {
                    child->didier[i]->set(j, l, m->get(j, l));
                }
            }
        }
        break;
    case 1:
        Matrix **source = b.didier;
        if (a.hasDidier) {
            source = a.didier;
        }
        for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
            Matrix::clone(source[i], child->didier[i]);
        }
        break;
    }

    return child;
}

Chromosome *swapPlayerCrossover(Chromosome &a, Chromosome &b) {
    Chromosome *child = new Chromosome();

    child->hasDidier = likelyness(0.5 * (a.hasDidier + b.hasDidier));

    for (int k = 0; k < EQUIPE_SIZE; k++) {
        Chromosome &source = likelyness(0.5) ? a : b;
        for (int i = 0; i < NETWORK_SIZE - 1; i++) {
            Matrix::clone(source.matrix[k][i], child->matrix[k][i]);
        }
    }
    return child;
}

Chromosome *crossover(Chromosome &a, Chromosome &b) {
    if (likelyness(SWAP_CROSSOVER_PROBA)) {
        return swapPlayerCrossover(a, b);
    }
    return classicCrossover(a, b);
}

void Chromosome::write(std::ofstream &file) {
    int equipeSize = EQUIPE_SIZE;
    int nSize = NETWORK_SIZE;
    int didierSize = DIDIER_NETWORK_SIZE;

    // Par sécurité
    file.write((char *)&equipeSize, sizeof(int));
    file.write((char *)&nSize, sizeof(int));
    file.write((char *)&didierSize, sizeof(int));

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
    int didierSize;

    // Par sécurité
    file.read((char *)&equipeSize, sizeof(int));
    file.read((char *)&nSize, sizeof(int));
    file.read((char *)&didierSize, sizeof(int));

    if (equipeSize != EQUIPE_SIZE || nSize != NETWORK_SIZE ||
        didierSize != DIDIER_NETWORK_SIZE) {
        std::cout << equipeSize << "≠" << EQUIPE_SIZE << " || " << nSize << "≠"
                  << NETWORK_SIZE << " || " << didierSize << "≠"
                  << DIDIER_NETWORK_SIZE << std::endl;
        throw std::invalid_argument(
            "Misconfigured Chromosome file (bad constants)");
    }

    const int pLayers[NETWORK_SIZE] = {};
    const int pDidierLayers[DIDIER_NETWORK_SIZE] = {};

    // Configs
    for (int i = 0; i < NETWORK_SIZE; i++) {
        file.read((char *)&pLayers[i], sizeof(int));
        if (pLayers[i] != PLAYER_LAYERS[i]) {
            std::cout << i << ":" << pLayers[i] << " ≠ " << PLAYER_LAYERS[i];
            throw std::invalid_argument(
                "Misconfigured Chromosome file (bad player layers)");
        }
    }
    for (int i = 0; i < DIDIER_NETWORK_SIZE; i++) {
        file.read((char *)&pDidierLayers[i], sizeof(int));
        if (pDidierLayers[i] != DIDIER_LAYERS[i]) {
            std::cout << i << ":" << pDidierLayers[i] << " ≠ "
                      << DIDIER_LAYERS[i];
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
