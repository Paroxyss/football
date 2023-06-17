#include <cstdlib>
#include <iostream>
#include <malloc/_malloc.h>
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
    Population next_pop = Population(this->size);
    int crossNumber = 0, mutationNumber = 0;

    std::cout << "While 1" << std::endl;
    while (crossNumber < this->size * 0.85) {
        auto cpl = this->tournament(rand() % (this->size / 2) + 2, save);
        std::cout << "gotChromosome " << cpl.first << " and " << cpl.second
                  << std::endl;

        // il y a certaines méthodes de crossover qui retournent directement 2
        // enfants par exemple one_pointer_crossover(a, b) !=
        // one_pointer_crossover(b, a) on peut donc avoir les 2;
        next_pop.pop[crossNumber] = crossover(cpl.first, cpl.second);
        crossNumber++;
    }

    std::cout << "While 2" << std::endl;
    while (mutationNumber < this->size * 0.1) {
        next_pop.pop[crossNumber] = mutate(this->pop[rand() % this->size]);
        crossNumber++;
        mutationNumber++;
    }

    std::cout << "While 3" << std::endl;
    while (crossNumber + mutationNumber < this->size) {
        next_pop.pop[crossNumber + mutationNumber] =
            this->pop[rand() % this->size];
        mutationNumber++;
    }

    std::cout << "While 4" << std::endl;
    for (int i = 0; i < this->size; i++) {
        this->pop[i] = next_pop.pop[i];
    }

    // ??? ça va delete les chromosomes
    // delete next_pop;
}

std::pair<Chromosome *, Chromosome *> Population::tournament(int tournamentSize,
                                                             bool save) {

    std::cout << "Tournament of " << tournamentSize << " players" << std::endl;
    Chromosome **r = (Chromosome **)malloc(tournamentSize * sizeof(void *));

    std::pair<Chromosome *, Chromosome *> p;

    for (int i = 0; i < tournamentSize; i++) {
        r[i] = this->pop[rand() % this->size];
        std::cout << "Added to r chromosome [" << r[i] << "]" << std::endl;
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
