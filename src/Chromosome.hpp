#pragma once

#include "Game.hpp"
#include "Matrix.h"
#include "config.h"

class Chromosome {
  public:
    Matrix *matrix[EQUIPE_SIZE][NETWORK_SIZE - 1];
    Matrix *didier[DIDIER_NETWORK_SIZE - 1];

    void initialize();
    ~Chromosome();

    void print();

    void randomize();

    Matrix *compute_didier(Matrix *inputs);
    Matrix *collect_and_evaluate(player *p, Matrix *didier);
    Matrix *evaluate(Matrix *inputs);
};

Chromosome *mutate(Chromosome *c);
Matrix *compute_didier(Chromosome *c, Matrix *inputs);
Chromosome *crossover(Chromosome *a, Chromosome *b);
