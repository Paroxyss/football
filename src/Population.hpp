#pragma once

#include "Chromosome.hpp"
#include <utility>

class Population {
  public:
    int size;
    Chromosome *pop;

  public:
    Population(int size);
    ~Population();
    Population *next();
    std::pair<Chromosome *, Chromosome *> tournament(int k);
};
