#pragma once

#include "Chromosome.hpp"
#include "Game.hpp"
#include <utility>

class Population {
  public:
    int size;
    Chromosome **pop;

  public:
    Population(int size);
    ~Population();

    void initialize();
    gameStatistics next(bool save = false);

    std::tuple<Chromosome *, Chromosome *, gameStatistics>
    tournament(int k, bool save);

    void write(std::ofstream &file);
    static Population *read(std::ifstream &file);
};

Chromosome **getChromosomeFromPopulations(Population **pop, unsigned int i);
Population *joinPopulation(Population **p, int n);
void shufflePopulations(Population **pop, unsigned int numberOfPop);
Chromosome *cloneChromosome(Chromosome *original);
