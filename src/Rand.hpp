#pragma once

#include <random>
#include <thread>

extern thread_local std::mt19937 rng;
int thrand(int min, int max);

double randomDouble(double min, double max);
double randomDouble();

bool likelyness(double v);
