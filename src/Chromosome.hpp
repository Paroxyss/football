#pragma once

#include "Game.hpp"
#include "Genealogy.hpp"
#include "Matrix.h"
#include "config.h"
#include <fstream>

typedef struct chromosomeStats {
    unsigned short instanceGoals;
    int instanceAge;
} chromosomeStats;

class Chromosome {
  public:
    chromosomeStats stats = {.instanceGoals = 0, .instanceAge = 0};
	unsigned long id;

    Matrix *matrix[EQUIPE_SIZE][NETWORK_SIZE - 1];
	Matrix *didier[DIDIER_NETWORK_SIZE - 1];

    Chromosome();
    ~Chromosome();

    void print();

    void initialize();

    void collect_and_apply(player *equipeAlliee, player *equipeAdverse,
                              ball *b, bool team);

    void apply(player *equipeAlliee);
	void apply_didier(player *equipeAlliee);

    void write(std::ofstream &file);
    static Chromosome *read(std::ifstream &file);

    double getPlayersNorm();
    double getMatrixesNorm();
    double getAngleNorm();
};

void mutate(Chromosome &c);
Chromosome *crossover(Chromosome &a, Chromosome &b);
double mmn(double x, double min, double max);
void normalize_inputs(Matrix &inputs, int i);
void writeInputs(player &target, player *equipeAlliee, player *equipeAdverse,
                 ball *b, bool team);
