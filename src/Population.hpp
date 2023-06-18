#pragma once

#include "Chromosome.hpp"
#include <utility>

class Population {
  public:
    int size;
    Chromosome **pop;

  public:
    Population(int size);
    ~Population();
    void next(bool save = false);
    std::pair<Chromosome *, Chromosome *> tournament(int k, bool save);
};

Chromosome **getChromosomeFromPopulations(Population *pop, unsigned int i);
void shufflePopulations(Population *pop, unsigned int numberOfPop);
Chromosome *cloneChromosome(Chromosome *original);
