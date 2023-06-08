#pragma once
#include "config.h"

struct player {
    double x, y, orientation, vitesse, size;
};

struct ball {
    double x, y, vx, vy, size;
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
    void setBall(double x, double y, double vx = 0, double vy = 0, double size = 1);

    void setPlayer(double x, double y, double speed = 0, double orientation = 0, double size = 0);

    void print();
};
