#include "Chromosome.hpp"

// Réinitialise un poids à une valeur aléatoire
// Permet d'explorer un large espace solution mais peut perdre une bonne
// solution rapidement.
double replacement();

// Varie légèrement autour de la valeur déjà contenu
// Permet de préserver la forme générale d'un individu déjà près d'une bonne
// solution. Le désavantage étant que l'exploration de l'espace solution est
// plus faible et la convergence plus lente.
double perturbation(double x);

// Tentative d'une solution équilibrée entre les deux valeurs précédentes
double balance(double x);

//Pour muter un chromosome (équipe) on mute chaque matrice composant le chromosome.
void mutate(Chromosome &c);
// Idem pour une matrice
void mutate(Matrix &c);
