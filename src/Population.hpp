#pragma once

#include <queue>

#include "Chromosome.hpp"
#include "Game.hpp"
#include "Generation.hpp"

class Population {
  public:
	int size;
	Chromosome **pop;

  public:
	Population(int size, double proportionDidier = 0.5);
	~Population();

	void initialize();
	gameStatistics next(int n_thread, bool save = false,
						Generation *parent = NULL);

	std::tuple<std::queue<std::pair<Chromosome *, double>>, gameStatistics>
	tournament(int tourn_size, int maxSize, bool save);

	void write(std::ofstream &file);
	static Population *read(std::ifstream &file);
};
void update_statistics(gameStatistics &tourn_stats,
					   gameStatistics *tournResult, int count);
