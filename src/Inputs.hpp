#pragma once

#include "Game.hpp"
#include "Matrix.h"

// Normalise x entre 0 et 1
double mmn(double x, double min, double max);

// Ecrit les entrées d'un joueur normalisées
void writeInputs(player &target, player *equipeAlliee,
				 player *equipeAdverse, ball *b, bool team);
