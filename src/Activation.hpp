#pragma once
#include "Matrix.h"
#include <cmath>

/*
 * J'ai lu que ReLu était le choix par défaut pour les fonction d'activation
 * mais que ça peut engendrer le problème de neurones mortes. Si ça arrives
 * il faut remplacer par leaky_ReLu.
 */

inline double a_tanh(double x) {
    return tanh(x);
}

inline double ReLu(double x) {
    return fmax(0, x);
}

inline double sigmoide(double x) {
    return 2 / (1 + exp(-x)) - 1;
}

inline double Heaviside(double x) {
    return x >= 0;
}

inline double leaky_ReLu(double x) {
    return fmax(0.01 * x, x);
}

inline double mish(double x) {
    return x * tanh(log(1 + exp(x)));
}

/**
 * @brief applique la fonction d'activation
 * pour les couches d'input.
 */
inline void input_layer_activation(Matrix &c) {
    for (int i = 0; i < c.ligne; i++) {
        for (int j = 0; j < c.col; j++) {
            c.set(i, j, ReLu(c.get(i, j)));
        }
    }
}

/**
 * @brief applique la fonction d'activation
 * pour les couches internes.
 */
inline void hidden_layer_activation(Matrix &c) {
    for (int i = 0; i < c.ligne; i++) {
        for (int j = 0; j < c.col; j++) {
            c.set(i, j, ReLu(c.get(i, j)));
        }
    }
}

/**
 * @brief applique les fonction d'activation
 * pour les couches de sorties.
 */
inline void output_layer_activation(Matrix &c) {
    for (int i = 0; i < c.ligne; i++) {
        for (int j = 0; j < c.col; j++) {
            c.set(i, j, tanh(c.get(i, j)));
        }
    }
}
