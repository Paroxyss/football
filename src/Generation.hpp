#pragma once

#include "Genealogy.hpp"
#include "util.hpp"

#include <fstream>

class Population;
class Generation {
  public:
    Population *currentPop = NULL;
    ArbreGenealogique arbre = ArbreGenealogique(0);
    std::list<gameStatistics> stats;
    unsigned int generation = 0;
    unsigned int nthread = 0;

  public:
    std::ofstream statsFile;
    Generation(unsigned int nthread);
    ~Generation();

    void create_population(unsigned int size);

    void step();
    void append_stats_file(gameStatistics g, int forceGen = -1);
    void rewrite_stats();

    // à la différence des autres classes, ici on peut charger et
    // sauvegarder les données au sein d'une même instance
    INFILE_METHOD(load)
    // INFILE_METHOD(loadPopulation)
    OUTFILE_METHOD(save)
    OUTFILE_METHOD(saveJson)
};

#include "Population.hpp"
