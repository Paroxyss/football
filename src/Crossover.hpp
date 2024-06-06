#pragma once

#include "Matrix.h"

Matrix *one_pointer_crossover(Matrix &a, Matrix &b);
Matrix *uniform_crossover(Matrix &a, Matrix &b);

Chromosome *crossover(Chromosome &a, Chromosome &b);
