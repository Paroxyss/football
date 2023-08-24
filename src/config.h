#pragma once

#define COM_SIZE 2

#define NETWORK_SIZE 4
#define DIDIER_NETWORK_SIZE 3

// pour l'instant le réseau ne prend pas l'accélération en entrée
// pour pouvoir évoluer plus facilement.
// x, y, vx, vy, theta, distance_balle, rthetab, distance cage, angle relatif
// cage, distance abverse + proche, angle adverse + proche, com1, .. ,
// com{COM_SIZE}
#define NETWORK_OUTPUT_SIZE (2 + COM_SIZE)
#define NETWORK_INPUT_SIZE (10 + COM_SIZE)

#define EQUIPE_SIZE 3

const int PLAYER_LAYERS[] = {NETWORK_INPUT_SIZE, 9, 9, NETWORK_OUTPUT_SIZE};
const int DIDIER_LAYERS[] = {COM_SIZE * EQUIPE_SIZE, 10, COM_SIZE *EQUIPE_SIZE};

#define GAME_RESOLUTION 1

#define GAME_DURATION (20 * 60)

#define MAP_LENGTH (119 * 10)
#define MAP_HEIGHT (75 * 10)
#define GOAL_HEIGHT (15 * 10)

#define PLAYER_MASS 100
#define PLAYER_SIZE (2 * 10)
#define PLAYER_ACCELERATION 0.5

#define BALL_MASS 40
#define BALL_SIZE 10

#define POPULATION_SIZE 1000

// entre 0.6 et 0.9
#define CROSSOVER_PROBABILITY 0.9

// entre 0.001 et 0.1
#define MUTATION_PROBABILITY 0.01
#define GENE_MUTATION_PROBABILITY 0.05

// entre 0.95 et 0.999
#define SAVE_POP_RATE 0.99

// nombre de générations
#define N 10
// intervalle (en génération) des sauvegardes
#define SAVE_RATE 100
