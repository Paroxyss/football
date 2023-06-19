#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

#include "Chromosome.hpp"
#include "Game.hpp"
#include "Population.hpp"
#include "config.h"

Population::Population(int size) {
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

void Population::next(bool save) {
    int tournament_size = this->size * PRESSION_TOURN;

    if (tournament_size <= 2) {
        throw std::invalid_argument("invalid population size");
    }

    Chromosome **next_pop = new Chromosome *[this->size];
    bool *toKeep = new bool[this->size];

    for (int i = 0; i < this->size; i++) {
        toKeep[i] = false;
    }

    int crossNumber = 0, mutationNumber = 0;

    while (crossNumber < this->size * 0.85) {
        auto cpl = this->tournament(rand() % (tournament_size - 2) + 2, save);

        next_pop[crossNumber] = crossover(cpl.first, cpl.second);
        next_pop[crossNumber + 1] = crossover(cpl.second, cpl.first);

        crossNumber += 2;
    }

    while (mutationNumber < this->size * 0.1) {
        int randIndice = rand() % this->size;

        next_pop[crossNumber + mutationNumber] = mutate(this->pop[randIndice]);
        toKeep[crossNumber + mutationNumber] = true;

        mutationNumber++;
    }

    while (crossNumber + mutationNumber < this->size) {
        int randIndice = rand() % this->size;

        next_pop[crossNumber + mutationNumber] =
            cloneChromosome(this->pop[randIndice]);
        mutationNumber++;
    }

    for (int i = 0; i < this->size; i++) {
        delete this->pop[i];
        this->pop[i] = next_pop[i];
    }

    delete[] toKeep;
    delete[] next_pop;
}

std::pair<Chromosome *, Chromosome *>
Population::tournament(int tournament_size, bool save) {
    std::pair<Chromosome *, Chromosome *> p;

    std::vector<Chromosome *> contestants(tournament_size);
    for (int i = 0; i < tournament_size; i++) {
        contestants[i] = this->pop[rand() % this->size];
    }

    while (tournament_size > 2) {
        int pool_size = tournament_size / 2 + (tournament_size % 2);
        std::vector<Chromosome *> pool(pool_size);

        for (int i = 0; i < pool_size; i++) {
            if (i * 2 + 1 == tournament_size) {
                pool[i] = contestants[i * 2];
            } else if (play_match(contestants[i * 2], contestants[i * 2 + 1],
                                  save) > 0) {
                pool[i] = contestants[i * 2];
            } else {
                pool[i] = contestants[i * 2 + 1];
            }
        }

        tournament_size = pool_size;
        contestants = pool;
    }

    p.first = contestants[0];
    p.second = contestants[1];

    return p;
}
Chromosome **getChromosomeFromPopulations(Population *pop, unsigned int i) {
    while (i >= pop->size) {
        i -= pop->size;
        pop += 1;
    }
    return &pop->pop[i];
};

void shufflePopulations(Population *pop, unsigned int numberOfPop) {
    int popTotale = 0;
    for (int i = 0; i < numberOfPop; i++) {
        popTotale += pop[i].size;
    }
    for (int i = popTotale - 1; i >= 0; i--) {
        int swapIndice = rand() % (i + 1);
        auto pc1 = getChromosomeFromPopulations(pop, i);
        auto pc2 = getChromosomeFromPopulations(pop, swapIndice);
        auto tmp = *pc1;
        *pc1 = *pc2;
        *pc2 = tmp;
    }
}

Chromosome *cloneChromosome(Chromosome *original) {
    auto clone = new Chromosome();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            for (int k = 0; k < original->matrix[i][j]->ligne; k++) {
                for (int l = 0; l < original->matrix[i][j]->col; l++) {
                    clone->matrix[i][j]->set(k, l,
                                             original->matrix[i][j]->get(k, l));
                }
            }
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        for (int k = 0; k < original->didier[i]->ligne; k++) {
            for (int l = 0; l < original->didier[i]->col; l++) {
                clone->didier[i]->set(k, l, original->didier[i]->get(k, l));
            }
        }
    }

    return clone;
}

Population *joinPopulation(Population *p, int n) {
    int c = 0;
    for (int i = 0; i < n; i++) {
        c += p[i].size;
    }

    auto np = new Population(c);
    int k = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p[i].size; j++) {
            delete np->pop[k];
            np->pop[k++] = cloneChromosome(p[i].pop[j]);
        }
    }

    return np;
}
