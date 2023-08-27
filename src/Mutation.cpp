#include "util.hpp"
#include <random>

/**
 * @brief Permet d'explorer un large espace solution mais peut perdre une bonne
 * solution rapidement.
 */
double replacement() {
    return randomDouble();
}

/**
 * @brief Permet de préserver la forme générale d'un individu déjà près d'une
 * bonne solution. Le désavantage étant que l'exploration de l'espace solution
 * est plus faible et la convergence plus lente.
 */
double perturbation(double x) {
    return x + randomDouble(-x / 10., x / 10.);
}

/**
 * @brief Juste une idée comme ça. Pour notre cas puisque l'on a pas une énorme
 * puissance de calcul on ne peut ni se permettre d'utiliser uniquement
 * replacement() ce qui requière une grosse population ni perturbation() qui
 * requière beaucoup de génération.
 */
double balance(double x) {
    if (likelyness(0.5)) {
        return perturbation(x);
    }

    return replacement();
}

double gaussian(double x) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> distribution(0.0, 0.1);

    return x + distribution(gen);
}
