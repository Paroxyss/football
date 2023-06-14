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
    double time;
    ball *actor;
    ball *secondary;
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

    void set_players(int conf[], int n, bool t);

    Game(int playerNumber);
    ~Game();

    void tick(double timeToAdvance = 1);
    bool tickBall(int id, double progress = 0);

    void doAction(unsigned int id, double rotation, double acceleration);

    void setBall(vector pos, vector vitesse = {.x = 0, .y = 0},
                 double size = BALL_SIZE, double mass = BALL_MASS);

    // liste les collisions futures d'un objet
    collisionList *getObjectCollisionList(int objId,
                                          collisionList *listToAppend = NULL);

    // bouge tous les objets du pourcentage/portion de tick donnée.
    void moveAllObj(double percent);

    void setPlayer(int id, vector pos, vector speed, double orientation,
                   double size = PLAYER_SIZE, double mass = BALL_MASS);

    void print();
};
