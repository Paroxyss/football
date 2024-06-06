#pragma once

#include <random>

extern thread_local std::mt19937 rng;
int thrand(int min, int max);

double random_double(double min, double max);
double random_double();

bool likelyness(double v);
