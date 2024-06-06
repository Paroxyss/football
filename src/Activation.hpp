#pragma once

#include <cmath>

#include "Matrix.hpp"

/*
   Remarques sur le choix de la fonction d'activation

   29/09:
	   J'ai lu que ReLu était le choix par défaut pour les fonction
   d'activation mais que ça peut engendrer le problème de neurones mortes.
   Si ça arrives il faut remplacer par leaky_ReLu.

   07/10:
	   Ni ReLu ni leaky_ReLu ne sont adaptées à notre problème, le problème
   du football est symétrique, il nous faut donc une fonction d'activation
	   symétrique, pour que les comportements le soient aussi.
	   On décide donc d'utiliser des sigmoides

   13/12:
	   Ajout d'un tanh adouci en sortie des réseaux de neuronnes pour
   permettre une plus grande précision à nos joueurs
 */

inline double a_tanh(double x) {
	return tanh(x);
}

inline double ReLu(double x) {
	return fmax(0, x);
}

inline double sigmoide(double x) {
	return 5 / (1 + exp(-x)) - 5 / 2.;
}

inline double heaviside(double x) {
	return x >= 0;
}

inline double leaky_ReLu(double x) {
	return fmax(0.01 * x, x);
}

inline double mish(double x) {
	return x * tanh(log(1 + exp(x)));
}

inline double tanh_adouci(double x) {
	return tanh((1. / 3.) * x);
}

// Applique la fonction d'activation pour les couches d'input.
inline void input_layer_activation(Matrix &c) {
	for (int i = 0; i < c.ligne; i++) {
		for (int j = 0; j < c.col; j++) {
			c.set(i, j, sigmoide(c.get(i, j)));
		}
	}
}

// Applique la fonction d'activation pour les couches internes.
inline void hidden_layer_activation(Matrix &c) {
	for (int i = 0; i < c.ligne; i++) {
		for (int j = 0; j < c.col; j++) {
			c.set(i, j, sigmoide(c.get(i, j)));
		}
	}
}

// Applique les fonction d'activation pour les couches de sorties.
// On adouci la sortie pour ameliorer la précision des joueurs
inline void output_layer_activation(Matrix &c) {
	for (int i = 0; i < c.ligne; i++) {
		for (int j = 0; j < c.col; j++) {
			c.set(i, j, tanh_adouci(c.get(i, j)));
		}
	}
}
