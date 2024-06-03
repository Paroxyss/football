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
#define NETWORK_OUTPUT_SIZE 3 + COM_SIZE
#define NETWORK_INPUT_SIZE 27 + COM_SIZE

#define EQUIPE_SIZE 3
#define GAMECONFIG                                                             \
    { 1, 0, 2 }
#define GAMECONFIGLENGTH 3

const int PLAYER_LAYERS[NETWORK_SIZE] = {NETWORK_INPUT_SIZE, 28, 20, 16, 10, 6,
                                         NETWORK_OUTPUT_SIZE};
const int DIDIER_LAYERS[DIDIER_NETWORK_SIZE] = {COM_SIZE * EQUIPE_SIZE,
                                                COM_SIZE * EQUIPE_SIZE};

// GAME CONFIG
#define MAX_GAME_DURATION (40 * 60)
#define MAX_TOUCH_DURATION (10 * 60)

// MAP CONFIG
#define MAP_LENGTH (119 * 20)
#define MAP_HEIGHT (75 * 20)
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

#define SHOOTSTRENGTH 50
#define SHOOTCOOLDOWN 10 // en seconde
 
#define COLLISION_CONS 0.9

// BALL CONFIG
#define BALL_MASS 10
#define BALL_SIZE 10
#define BALL_FROTTEMENT (1 / 35.)

// SENSOR CONFIG
// Tests d'une autre methode de communication: les capteurs, 
// pour conserver la continuité des entrées 
// (abandonnee car trop complexe a apprendre)
#define PLAYER_SENSOR_NUMBER 3
const double PLAYER_SENSOR_ANGLES[PLAYER_SENSOR_NUMBER] = {
    -45. * M_PI / 180., 0. * M_PI / 180., 45. * M_PI / 180.};
#define PLAYER_SENSOR_DISTANCE 4 * PLAYER_SIZE
#define PLAYER_SENSOR_RANGE 8 * PLAYER_SIZE

#define POPULATION_SIZE 1000

// entre 0 et 1, configure la taille maximum des tournois en proportion de
// population, 1 pour tous, 0 pour personne
#define PRESSION_SELECTIVE 0.5

// Nombre de joueurs créés par tournois
#define NB_PAR_TOURNOI 2

// Probabilite de crossover classique (moyenne)
#define CROSSOVER_PROBABILITY 0.2 // 0.9
// Probabilite de crossover mixte (mixe les deux équipes)
#define SWAP_CROSSOVER_PROBA 0.25

// Probabilite de faire varier les poids d'un joueur
// entre 0.001 et 0.1
#define MUTATION_PROBABILITY 0.04

// Probablite d'echanger des poids entre les joueurs, au sein d'une meme equipe
#define SWAP_MUTATION_PROBA 0.025

// Proportion de joueurs aléatoires par générations
#define NEW_BLOOD 0.1

// intervalle (en génération) des sauvegardes
#define SAVE_RATE 5

// Active les tests lors de l'accession aux matrices (tres couteux en ressources)
// #define MATRIXDEBUG
