#pragma once

#include "Chromosome.hpp"
#include "Game.hpp"
#include "Generation.hpp"
#include <queue>
#include <utility>

class Population {
  public:
    int size;
    Chromosome **pop;

  public:
    Population(int size);
    ~Population();

    void initialize();
    gameStatistics next(int n_thread, bool save = false,
                        Generation *parent = NULL);

    std::tuple<std::queue<Chromosome *>, gameStatistics>
    tournament(int tourn_size, int maxSize, bool save);

    void write(std::ofstream &file);
    static Population *read(std::ifstream &file);
};
Chromosome *cloneChromosome(Chromosome *original);
void update_statistics(gameStatistics &tourn_stats, gameStatistics *tournResult,
                       int count);
