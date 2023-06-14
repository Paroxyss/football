#pragma once
#define PI 3.1415

#define NETWORK_SIZE 4
#define NETWORK_OUTPUT_SIZE 5
#define NETWORK_INPUT_SIZE 4

#define EQUIPE_SIZE 3
#define DIDIER_NETWORK_SIZE 5

#define COM_SIZE 1

const int PLAYER_LAYERS[] = {NETWORK_INPUT_SIZE, 15, 25, NETWORK_OUTPUT_SIZE};
const int DIDIER_LAYERS[] = {COM_SIZE * EQUIPE_SIZE, 10, 15, 20,
                             COM_SIZE *EQUIPE_SIZE};

#define GAME_RESOLUTION 1

#define MAP_LENGTH 2000
#define MAP_HEIGHT 1000
#define GOAL_HEIGHT 25

#define PLAYER_MASS 100
#define PLAYER_SIZE 100
#define PLAYER_ACCELERATION 1

#define BALL_MASS 100
#define BALL_SIZE 100
