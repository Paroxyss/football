#pragma once

#include "Chromosome.hpp"
#include "Matrix.hpp"
#include "Rand.hpp"

// ## CROSSOVERS ENTRE MATRICES

// Place une barre verticale sur la matrice, remplis à gauche de la barre
// avec les valeurs de A et à droite avec les valeurs de B
Matrix *one_pointer_crossover(Matrix &a, Matrix &b);

// Fait un pile ou face pour selectionner la source (A ou B) de chaque
// coefficient de la matrice
Matrix *uniform_crossover(Matrix &a, Matrix &b);

// ## CROSSOVERS ENTRE CHROMOSOMES

// Réalise un uniform_crossover sur chaque matrice du chromosome
Chromosome *classicCrossover(Chromosome &a, Chromosome &b);

// Forme un nouveau chromosome en prenant chaque joueur dans une des deux
// équipe aléatoirement
Chromosome *swapPlayerCrossover(Chromosome &a, Chromosome &b);

// Choisit aléatoirement la méthode de crossover selon les probabilités
// spécifiées dans config
Chromosome *crossover(Chromosome &a, Chromosome &b);
