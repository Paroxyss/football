#pragma once

#define COM_SIZE 1

#define NETWORK_SIZE 4
#define DIDIER_NETWORK_SIZE 3

// pour l'instant le réseau ne prend pas l'accélération en entrée
// pour pouvoir évoluer plus facilement.
// x, y, vx, vy, theta, distance_balle, rthetab, com1, .. , com{COM_SIZE}
#define NETWORK_OUTPUT_SIZE 2 + COM_SIZE
#define NETWORK_INPUT_SIZE 7 + COM_SIZE

#define EQUIPE_SIZE 3

const int PLAYER_LAYERS[] = {NETWORK_INPUT_SIZE, 10, 10, NETWORK_OUTPUT_SIZE};
const int DIDIER_LAYERS[] = {COM_SIZE * EQUIPE_SIZE, 5, COM_SIZE *EQUIPE_SIZE};

#define GAME_RESOLUTION 1

#define GAME_DURATION 60 * 60

#define MAP_LENGTH 119 * 10
#define MAP_HEIGHT 75 * 10
#define GOAL_HEIGHT 15 * 10

#define PLAYER_MASS 100
#define PLAYER_SIZE 2 * 10
#define PLAYER_ACCELERATION 0.5

#define BALL_MASS 40
#define BALL_SIZE 10

#define PRESSION_TOURN 0.1

#define LOGGAME false
