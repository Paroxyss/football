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
    gameStatistics next(int n_thread, bool save = false);

    std::tuple<Chromosome *, Chromosome *, gameStatistics>
    tournament(int k, bool save);

    void write(std::ofstream &file);
    static Population *read(std::ifstream &file);
};

void thread_tournament(
    int tourn_size, bool save,
    std::tuple<Chromosome *, Chromosome *, gameStatistics> *wheretowrite,
    Population *pop);

Chromosome *cloneChromosome(Chromosome *original);
