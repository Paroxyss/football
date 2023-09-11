#pragma once

#define NETWORK_SIZE 4

// pour l'instant le réseau ne prend pas l'accélération en entrée
// pour pouvoir évoluer plus facilement.
// x, y, vx, vy, theta, distance_balle, rthetab, distance cage, angle relatif
// cage, distance abverse + proche, angle adverse + proche, com1, .. ,
// com{COM_SIZE}
#define NETWORK_OUTPUT_SIZE 2
#define NETWORK_INPUT_SIZE 10

#define EQUIPE_SIZE 1

const int PLAYER_LAYERS[] = {NETWORK_INPUT_SIZE, 8, 6, NETWORK_OUTPUT_SIZE};

#define GAME_RESOLUTION 1

#define GAME_DURATION (40 * 60)

#define MAP_LENGTH (119 * 5)
#define MAP_HEIGHT (75 * 5)
#define GOAL_HEIGHT (15 * 10)

#define PLAYER_MASS 100
#define PLAYER_SIZE (2 * 10)
#define PLAYER_ACCELERATION 1
#define PLAYER_FROTTEMENT (1 / 100.)

#define BALL_MASS 40
#define BALL_SIZE 10
#define BALL_FROTTEMENT (1 / 100.)

#define POPULATION_SIZE 1000

// entre 0.6 et 0.9
#define CROSSOVER_PROBABILITY 0.9

// entre 0.001 et 0.1
#define MUTATION_PROBABILITY 0.001

#define NEW_BLOOD 0.1

// nombre de générations
#define N 10
// intervalle (en génération) des sauvegardes
#define SAVE_RATE 100
