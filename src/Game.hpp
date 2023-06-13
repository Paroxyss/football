#pragma once
#include "Vector.hpp"
#include "config.h"

struct ball {
    vector pos, vitesse;
    double size, mass;
};

struct player : ball {
    double orientation, acceleration;
};

// alors oui c'est absolument honteux, mais le polymorphisme c'est la méga fête.
// pos est le points d'origine, vitesse le vecteur directeur, et size pourra
// déterminer la longueur (non prise en compte pour le moment)
struct wall : ball {};

enum CollisionType { WALL, CIRCLE };

struct collisionList {
    ball *obj;
    CollisionType type;
    collisionList *next;
};

struct backtrackCollisionReport {
    double distanceBefore;
    collisionList *obj;
};

class Game {
  public:
    player *players;
    unsigned int playerNumber;
    ball ball;

    unsigned int wallNumber;
    wall *walls;

    Game(int playerNumber);
    ~Game();

    void tick();
    void tickBall(int id, double progress = 0);

    void doAction(unsigned int id, double rotation, double acceleration);

    void setBall(vector pos, vector vitesse = {.x = 0, .y = 0},
                 double size = BALL_SIZE, double mass = BALL_MASS);

    collisionList *getCollisionList(int objId);

    void setPlayer(int id, vector pos, vector speed, double orientation,
                   double size = PLAYER_SIZE, double mass = BALL_MASS);

    void print();
};

backtrackCollisionReport backTrackCollisions(collisionList *collision,
                                             ball &obj);
void computeCollision(collisionList *collision, ball &b);
