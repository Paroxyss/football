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
    void next(bool save);
    std::pair<Chromosome *, Chromosome *> tournament(int k, bool save);
};
