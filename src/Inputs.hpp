#pragma once

#include "Game.hpp"
#include "Matrix.hpp"

// Normalise x entre 0 et 1
double mmn(double x, double min, double max);

// Ecrit les entrées d'un joueur normalisées
void write_inputs(player &viewer, player *equipeAlliee,
                  player *equipeAdverse, ball *b, bool team);
