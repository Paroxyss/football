#pragma once
#include "Generation.hpp"

// Entraine une génération déjà instanciée
void trainPop(Generation &g, int n_gen, int n_thread);

// Charge une population depuis un fichier et poursuit l'entrainement
void trainFromFile(const char *inputFile, int n_gen, int population_size,
				   int n_thread);

// Crée une nouvelle population et commence un entrainement dessus
void train(int n_gen, int pop_size, int n_thread);
