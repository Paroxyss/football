#pragma once

#include "Game.hpp"
#include "Matrix.h"
#include "config.h"
#include <fstream>

class Chromosome {
  public:
    Matrix *matrix[EQUIPE_SIZE][NETWORK_SIZE - 1];
    Matrix *didier[DIDIER_NETWORK_SIZE - 1];

    Chromosome();
    ~Chromosome();

    void print();

    void initialize();

    void apply_didier(Matrix &inputs);

    Matrix *collect_and_apply(player *equipeAlliee, player *equipeAdverse,
                              ball *b, Matrix &didier_output, bool team);

    Matrix *apply(player *equipeAlliee);

    void write(std::ofstream &file);
    static Chromosome *read(std::ifstream &file);

    double getPlayersNorm();
    double getDidierNorm();
    double getMatrixesNorm();
    double getAngleNorm();
    std::pair<double, double> get2dProjection();
};

void mutate(Chromosome &c);
Matrix *compute_didier(Chromosome *c, Matrix *inputs);
Chromosome *crossover(Chromosome &a, Chromosome &b);
void writeInputs(player *equipeAlliee, player *equipeAdverse, int i,
                 ball *b, bool team);
