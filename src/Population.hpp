#pragma once

#include "Chromosome.hpp"
#include "Game.hpp"
#include <utility>
#include "Generation.hpp"

class Population {
  public:
    int size;
    Chromosome **pop;

  public:
    Population(int size);
    ~Population();

    void initialize();
    gameStatistics next(int n_thread, bool save = false, Generation *parent = NULL);

    std::tuple<Chromosome *, Chromosome *, gameStatistics>
    tournament(int k, bool save);

    void write(std::ofstream &file);
    static Population *read(std::ifstream &file);
};
Chromosome *cloneChromosome(Chromosome *original);
void update_statistics(gameStatistics &tourn_stats, gameStatistics *tournResult,
                       int count);

