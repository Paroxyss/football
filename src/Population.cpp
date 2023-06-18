#include <cstdlib>
#include <iostream>
#include <utility>

#include "Chromosome.hpp"
#include "Game.hpp"
#include "Population.hpp"
#include "config.h"

Population::Population(int size) {
    if (size * PRESSION_TOURN < 2) {
        throw std::invalid_argument("size must be greater than 20");
    }

    this->size = size;
    this->pop = new Chromosome *[size];

    for (int i = 0; i < size; i++) {
        this->pop[i] = new Chromosome();
        this->pop[i]->randomize();
    }
}

Population::~Population() {
    for (int i = 0; i < size; i++) {
        delete this->pop[i];
    }

    delete[] this->pop;
}

/*
    De ce que je me souvent une m�thode classique est de g�n�ration
    à chaque générations entre 80 et 90% des individus par reproduction,
    1% par mutation et le reste par clonage.
*/

void Population::next(bool save) {
    Chromosome **next_pop = new Chromosome *[this->size];
    bool *toKeep = new bool[this->size];

    for (int i = 0; i < this->size; i++) {
        toKeep[i] = false;
    }

    int crossNumber = 0, mutationNumber = 0;

    while (crossNumber < this->size * 0.85) {
        int ts = static_cast<int>(this->size * PRESSION_TOURN);
        auto cpl = this->tournament(rand() % (ts - 2) + 2, save);

        next_pop[crossNumber] = crossover(cpl.first, cpl.second);
        next_pop[crossNumber + 1] = crossover(cpl.second, cpl.first);

        crossNumber += 2;
    }

    while (mutationNumber < this->size * 0.1) {
        int randIndice = rand() % this->size;

        next_pop[crossNumber + mutationNumber] = mutate(this->pop[randIndice]);
        toKeep[crossNumber+mutationNumber] = true;

        mutationNumber++;
    }

    while (crossNumber + mutationNumber < this->size) {
        int randIndice = rand() % this->size;

        next_pop[crossNumber + mutationNumber] = cloneChromosome(this->pop[randIndice]);
        mutationNumber++;
    }

    for (int i = 0; i < this->size; i++) {
        delete this->pop[i];
        this->pop[i] = next_pop[i];
    }

    delete[] toKeep;
    delete[] next_pop;
}

std::pair<Chromosome *, Chromosome *> Population::tournament(int tournamentSize,
                                                             bool save) {
    Chromosome **r = new Chromosome *[tournamentSize];

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

    delete[] r;

    return p;
}

// renvoie un pointeur vers celui du i-eme chromosome d'un tableau de
// populations
Chromosome **getChromosomeFromPopulations(Population *pop, unsigned int i) {
    while (i >= pop->size) {
        i -= pop->size;
        // on avance sur la population suivante (arithm�tique de pointeurs)
        pop += 1;
    }
    return &pop->pop[i];
};

// m�lange en place les chromosomes d'un tableau de populations
void shufflePopulations(Population *pop, unsigned int numberOfPop) {
    int popTotale = 0;
    for (int i = 0; i < numberOfPop; i++) {
        popTotale += pop[i].size;
    }
    for (int i = popTotale - 1; i >= 0; i--) {
        // on pourrait d�caler l'indice de la boucle mais �a me semble moins
        // clair
        int swapIndice = rand() % (i + 1);
        auto pc1 = getChromosomeFromPopulations(pop, i);
        auto pc2 = getChromosomeFromPopulations(pop, swapIndice);
        auto tmp = *pc1;
        *pc1 = *pc2;
        *pc2 = tmp;
    }
}

Chromosome *cloneChromosome(Chromosome *original){
    auto clone = new Chromosome();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            for(int k = 0; k < original->matrix[i][j]->ligne; k++){
                for(int l = 0; l < original->matrix[i][j]->col; l++) {
                    clone->matrix[i][j]->set(k,l, original->matrix[i][j]->get(k,l));
                }
            }
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        for(int k = 0; k < original->didier[i]->ligne; k++){
            for(int l = 0; l < original->didier[i]->col; l++) {
                clone->didier[i]->set(k,l, original->didier[i]->get(k,l));
            }
        }
    }

    return clone;
}