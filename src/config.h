#pragma once

#include <cmath>
#define NETWORK_SIZE 7
#define DIDIER_NETWORK_SIZE 2

// pour l'instant le réseau ne prend pas l'accélération en entrée
// pour pouvoir évoluer plus facilement.
// x, y, vx, vy, theta, distance_balle, rthetab, distance cage, angle relatif
// cage, distance abverse + proche, angle adverse + proche, com1, .. ,
// com{COM_SIZE}
#define COM_SIZE 2
#define NETWORK_OUTPUT_SIZE 2 + COM_SIZE
#define NETWORK_INPUT_SIZE 26 + COM_SIZE

#define EQUIPE_SIZE 3
#define GAMECONFIG                                                             \
    { 1, 2 }
#define GAMECONFIGLENGTH 2

const int PLAYER_LAYERS[NETWORK_SIZE] = {NETWORK_INPUT_SIZE, 28, 20, 16, 10, 6,
                                         NETWORK_OUTPUT_SIZE};
const int DIDIER_LAYERS[DIDIER_NETWORK_SIZE] = {COM_SIZE * EQUIPE_SIZE,
                                                COM_SIZE *EQUIPE_SIZE};

// GAME CONFIG
#define MAX_GAME_DURATION (40 * 60)
#define MAX_TOUCH_DURATION (10 * 60)

// MAP CONFIG
#define MAP_LENGTH (119 * 7)
#define MAP_HEIGHT (75 * 7)
#define GOAL_HEIGHT (15 * 12)
#define POTEAU_LENGTH 20
#define POTEAU_WIDTH 20

// PLAYER CONFIG
#define PLAYER_MASS 100
#define PLAYER_SIZE (2 * 10)
#define PLAYER_ACCELERATION 0.30
#define PLAYER_FROTTEMENT (1 / 20.)
#define PLAYER_ROTATION_ACCELERATION 0.10
#define PLAYER_ROTATION_FROTTEMENT (1 / 3.)
#define COLLISION_CONS 1

// SENSOR CONFIG
#define PLAYER_SENSOR_NUMBER 3
const double PLAYER_SENSOR_ANGLES[PLAYER_SENSOR_NUMBER] = {
    -45. * M_PI / 180., 0. * M_PI / 180., 45. * M_PI / 180.};
#define PLAYER_SENSOR_DISTANCE 4 * PLAYER_SIZE
#define PLAYER_SENSOR_RANGE 8 * PLAYER_SIZE

#define BALL_MASS 10
#define BALL_SIZE 10
#define BALL_FROTTEMENT (1 / 35.)

#define POPULATION_SIZE 1000

// entre 0 et 1, configure la taille maximum des tournois en proportion de
// population, 1 pour tous, 0 pour personne
#define PRESSION_SELECTIVE 0.5
#define NB_PAR_TOURNOI 2
// entre 0.6 et 0.9
#define CROSSOVER_PROBABILITY 0.9
#define SWAP_MUTATION_PROBA 0.025

// entre 0.001 et 0.1
#define MUTATION_PROBABILITY 0.001

#define SWAP_CROSSOVER_PROBA 0.25

#define NEW_BLOOD 0.1

// nombre de générations
#define N 10
// intervalle (en génération) des sauvegardes
#define SAVE_RATE 5

// #define MATRIXDEBUG
