#include <cstdlib>
#include <iostream>
#include <utility>

#include "Chromosome.hpp"
#include "Game.hpp"
#include "Population.hpp"
#include "config.h"

Population::Population(int size) {
    this->size = size;
    this->pop = (Chromosome **)malloc(size * sizeof(void *));

    for (int i = 0; i < size; i++) {
        this->pop[i] = new Chromosome();
        this->pop[i]->randomize();
    }
}

Population::~Population() {
    for (int i = 0; i < size; i++) {
        std::cout << "freeing chromosome " << i << std::endl;
        delete this->pop[i];
        std::cout << "ok" << i << std::endl;
    }
    free(this->pop);
}

/*
    De ce que je me souvent une méthode classique est de généré
    à chaque générations entre 80 et 90% des individus par reproduction,
    1% par mutation et le reste par clonage.
*/

void Population::next(bool save) {
    Chromosome **next_pop = (Chromosome **)malloc(this->size * sizeof(void *));
    bool *toKeep = (bool *)malloc(this->size * sizeof(bool));

    for (int i = 0; i < this->size; i++) {
        toKeep[i] = false;
    }

    int crossNumber = 0, mutationNumber = 0;

    while (crossNumber < this->size * 0.85) {
        // le tournois ne doit pas être trop grand: si on prend 50% de la
        // population totale alors 1 fois sur 2 le même individu (le meilleur
        // des 50% sortira) et on obtiendra seulement 2 individus différents à
        // la fin, de plus les petits tournois sont beaucoup plus rapide à faire
        // tourner.
        int ts = this->size * 0.2;
        auto cpl = this->tournament(rand() % ts + 2, save);

        // chaque couple de parents produit 2 enfants, ce qui nécessite 2 fois
        // moins de tournois.
        next_pop[crossNumber] = crossover(cpl.first, cpl.second);
        next_pop[crossNumber + 1] = crossover(cpl.second, cpl.first);

        crossNumber += 2;
    }

    while (mutationNumber < this->size * 0.1) {
        int randIndice = rand() % this->size;

        next_pop[crossNumber + mutationNumber] = mutate(this->pop[randIndice]);

        mutationNumber++;
    }

    while (crossNumber + mutationNumber < this->size) {
        int randIndice = rand() % this->size;

        next_pop[crossNumber + mutationNumber] = this->pop[randIndice];
        toKeep[randIndice] = true;
        mutationNumber++;
    }

    for (int i = 0; i < this->size; i++) {
        if (!toKeep[i]) {
            // le chromosome ne sera pas conservé, on peut le free
            delete this->pop[i];
        }
        this->pop[i] = next_pop[i];
    }

    free(toKeep);
    free(next_pop);

    // ??? ça va delete les chromosomes
    // delete next_pop;
}

/*   ⠀⠀⠀⠀⣀⣤⣤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⡿⠿⣿⣿⣿⣿⡿⠿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢀⣴⣦⡈⠻⣦⣤⣿⣿⣧⣤⣶⠏⢀⣦⣄⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣷⣤⣈⠙⠛⠛⠛⢉⣠⣴⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀
⠀⠀⠀⠀⢠⣿⣿⣿⣿⠟⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⢻⣿⣿⣿⣆⠀⠀⠀⠀
⠀⠀⠀⢀⣿⣿⣿⣿⠃⣰⣿⣿⡿⠛⠋⠉⠛⠻⣿⣿⣷⡀⠹⣿⣿⣿⡆⠀⠀⠀
⠀⠀⠀⣸⣿⣿⣿⠃⣰⣿⣿⠋⣠⣾⡇⢸⣷⣦⠈⣿⣿⣿⡄⢹⣿⣿⣿⠀⠀⠀
⠀⠀⠀⣿⣿⣿⠋⠀⠉⠉⠉⠀⣿⣿⡇⢸⣿⣿⡇⠉⠉⠉⠁⠀⢻⣿⣿⡆⠀⠀
⠀⠀⢰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀
⠀⠀⠀⠙⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠃⠘⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠁⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠃⠀⠀⠀⠀⠀⠀⠀⠀
Prions pour que ça marche
*/
std::pair<Chromosome *, Chromosome *> Population::tournament(int tournamentSize,
                                                             bool save) {

    std::cout << "Tournament of " << tournamentSize << " players" << std::endl;
    Chromosome **r = (Chromosome **)malloc(tournamentSize * sizeof(void *));

    std::pair<Chromosome *, Chromosome *> p;

    for (int i = 0; i < tournamentSize; i++) {
        r[i] = this->pop[rand() % this->size];
    }

    for (int i = 1; i < tournamentSize; ++i) {
        Chromosome *c = r[i];
        int j = i - 1;

        while (j >= 0 && play_match(r[j], c, save) > 0) {
            r[j + 1] = r[j];
            --j;
        }

        r[j + 1] = c;
    }

    p.first = r[tournamentSize - 1];
    p.second = r[tournamentSize - 2];

    free(r);

    return p;
}
