#pragma once
#include "Vector.hpp"
#include "config.h"

struct player {
	vector pos, vitesse;
    double orientation, acceleration, size;
};

struct ball {
	vector pos, vitesse;
    double size;
};

class Game {
  public:
    player *players;
    unsigned int playerNumber;
    ball ball;

    Game(int playerNumber);
    ~Game();
	
    void tick();

    void doAction(unsigned int id, double rotation, double acceleration);
	
    void setBall(vector pos, vector vitesse = {.x = 0, .y = 0}, double size = BALL_SIZE);

	void setPlayer(int id, vector pos, vector speed, double orientation, double size = PLAYER_SIZE);

    void print();
};
