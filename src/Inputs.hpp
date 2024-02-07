#pragma once
#include "Game.hpp"
#include "Matrix.h"

double mmn(double x, double min, double max);
void normalize_inputs(Matrix &inputs, int i);
void writeInputs(player &target, player *equipeAlliee, player *equipeAdverse,
                 ball *b, bool team);
