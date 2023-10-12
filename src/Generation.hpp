#pragma once
#include <fstream>
#include <string>
#include <vector>

#include "Game.hpp"
#include "Genealogy.hpp"
#include "util.hpp"

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

    void createPopulation(unsigned int size);

    void step();
	void appendStatsFile(gameStatistics g, int forceGen = -1);
    void rewriteStats();

    // à la différence des autres classes, ici on peut charger et sauvegarder
    // les données au sein d'une même instance
    INFILE_METHOD(load)
    // INFILE_METHOD(loadPopulation)
    OUTFILE_METHOD(save)
    OUTFILE_METHOD(saveJson)
};

#include "Population.hpp"
