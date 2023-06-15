#pragma once

#include "Matrix.h"

double a_tanh(double x);
double ReLu(double x);
double sigmoide(double x);
double Heaviside(double x);
void apply_activation(Matrix *c);
