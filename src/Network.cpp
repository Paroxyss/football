#include "Network.hpp"

#include "Matrix.h"
#include "config.h"
#include "util.hpp"

#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>

// Génère un réseau remplit de 0, avec les largeurs données.
Network::Network(std::array<unsigned int, NETWORK_SIZE> lSizes) {
    this->weight = (Matrix **)malloc(NETWORK_SIZE * sizeof(void *));
    for (int i = 0; i < NETWORK_SIZE - 1; i++) {
        this->weight[i] = new Matrix(lSizes[i + 1], lSizes[i]);
    }
}

Network::~Network() {
    for (int i = 0; i < NETWORK_SIZE - 1; i++) {
        delete this->weight[i];
    }
    free(this->weight);
}

// Évalue le réseau sur un tableau d'entrées données
std::array<double, NETWORK_OUTPUT_SIZE> Network::evaluate(std::array<double, NETWORK_INPUT_SIZE> input) {
    Matrix actualVal(NETWORK_INPUT_SIZE, 1);

    for (int i = 0; i < NETWORK_INPUT_SIZE; i++) {
        actualVal.set(i, 0, input[i]);
    }

    for (int i = 0; i < NETWORK_SIZE - 1; i++) {
        actualVal.mult_inv(this->weight[i]);

        for (unsigned i = 0; i < actualVal.ligne; ++i) {
            for (unsigned j = 0; j < actualVal.col; ++j) {
                actualVal.set(i, j, tanh(actualVal.get(i, j)));
            }
        }
    }

    std::array<double, NETWORK_OUTPUT_SIZE> output;

    for (int i = 0; i < NETWORK_OUTPUT_SIZE; i++) {
        output[i] = actualVal.get(i, 1);
    }

    return output;
}

// deviation is in proportion
void Network::mutate(double rate, double deviation) {
    for (int a = 0; a < NETWORK_SIZE - 1; a++) {
        for (unsigned i = 0; i < this->weight[a]->ligne; ++i) {
            for (unsigned j = 0; j < this->weight[a]->col; ++j) {
                if (abs(randomDouble()) < rate) { // on mute
                    auto coef = 1. + deviation * randomDouble();
                    this->weight[a]->set(i, j, this->weight[a]->get(i, j) * coef + 0.01);
                }
            }
        }
    }
}

// Remplit le réseau de poids aléatoires
void Network::randomize() {
    for (int a = 0; a < NETWORK_SIZE - 1; a++) {
        for (unsigned i = 0; i < this->weight[a]->ligne; ++i) {
            for (unsigned j = 0; j < this->weight[a]->col; ++j) {
                this->weight[a]->set(i, j, (double)(randomDouble() * 10));
            }
        }
    }
}

// Affiche le réseau
void Network::print() {
    std::cout << "Network : " << std::endl;
    for (int a = 0; a < NETWORK_SIZE - 1; a++) {
        std::cout << "============" << std::endl;
        this->weight[a]->print();
    }
}
