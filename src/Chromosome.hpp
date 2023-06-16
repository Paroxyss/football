#pragma once

#include "Game.hpp"
#include "Matrix.h"
#include "config.h"

class Chromosome {
  public:
    Matrix *matrix[EQUIPE_SIZE][NETWORK_SIZE - 1];
    Matrix *didier[DIDIER_NETWORK_SIZE - 1];

    Chromosome();
    ~Chromosome();

    void print();

    void randomize();

    void apply_didier(Matrix &inputs);
	
    Matrix *collect_and_apply(player *p, Matrix &didier);
	
    void apply(Matrix &inputs);
};

Chromosome *mutate(Chromosome *c);
Matrix *compute_didier(Chromosome *c, Matrix *inputs);
Chromosome *crossover(Chromosome *a, Chromosome *b);
