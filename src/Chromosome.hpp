#pragma once

#include "Game.hpp"
#include "Matrix.h"
#include "config.h"
#include <fstream>

class Chromosome {
  public:
    Matrix *matrix[EQUIPE_SIZE][NETWORK_SIZE - 1];

    Chromosome();
    ~Chromosome();

    void print();

    void initialize();

    Matrix *collect_and_apply(player *equipeAlliee, player *equipeAdverse,
                              ball *b, bool team);

    Matrix *apply(player *equipeAlliee);

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
void writeInputs(player &target, player *equipeAdverse, ball *b, bool team);
