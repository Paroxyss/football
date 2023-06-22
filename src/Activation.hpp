#pragma once
#include "Matrix.h"
#include <cmath>

/*
    Il se trouve que le problème par lequel les jours ne bougent pas
    est tellement connu qu'il porte un nom:
        https://en.wikipedia.org/wiki/Vanishing_gradient_problem

    Pour résumer si on initialise à de petites valeurs ou que la fonction
   d'activation retourne une petite valeur alors après les multiplication des
   matrices de chaque layer on obtient des valeurs très petit et on dit alors
   que le réseau est "mort".

    De ce que j'ai comprit pour éviter le problème il faut utiliser des
   fonctions comme ReLu ou leaky ReLu (au moins pour les hidden layers) à la
   place de tanh.
*/

inline double a_tanh(double x) {
    return tanh(x - 4);
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

inline void apply_activation(Matrix &c) {
    for (int i = 0; i < c.ligne; i++) {
        for (int j = 0; j < c.col; j++) {
            c.set(i, j, leaky_ReLu(c.get(i, j)));
        }
    }
}

inline void output_activation(Matrix &c) {
    for (int i = 0; i < c.ligne; i++) {
        for (int j = 0; j < c.col; j++) {
            c.set(i, j, tanh(c.get(i, j)));
        }
    }
}
