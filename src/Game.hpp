#pragma once
#include "Matrix.h"
#include "Vector.hpp"
#include "config.h"
#include <fstream>

struct ball {
    // pos en m, vitesse en m.s-1
    vector pos, vitesse;
    // size le rayon en m, mass en kg
    double size, mass;
};

struct player : ball {
    // orientation en radian, acceleration en m.s-2
    double orientation, acceleration;
    Matrix *inputs;
};

// faut la mettre ici sinon ça compile pas (mdr)
#include "Chromosome.hpp"

// il faut le me mettre ici sinon ça complie pas (mdr)
class Chromosome;

// alors oui c'est absolument honteux, mais le polymorphisme c'est la méga fête.
// pos est le points d'origine, vitesse le vecteur directeur, et size pourra
// déterminer la longueur (non prise en compte pour le moment)
struct wall : ball {};

enum CollisionType { WALL, CIRCLE };

struct collisionList {
    // temps auquel la collision arrive (en s)
    double time;
    // Acteur de la collision
    ball *actor;
    // Objet la subissant
    ball *secondary;
    CollisionType type;
    collisionList *next;
};

extern std::ofstream csvOutputFile;

class Game {
  public:
    bool logToFile;

    player *players;
    unsigned int playerNumber;
    ball ball;

    unsigned int wallNumber;
    wall *walls;
    wall *goals;

    gameInformations infos = {.collisions = 0, .goals = 0, .score = 0};

    void set_players(const int conf[], int n);

    Game(int playerNumber, bool logToFile = false);
    ~Game();

    // fait avancer la simulation du temps souhaité
    void tick(double timeToAdvance = 1, bool root = true);

    void doAction(unsigned int id, double rotation, double acceleration);

    void setBall(vector pos, vector vitesse = {.x = 0, .y = 0},
                 double size = BALL_SIZE, double mass = BALL_MASS);

    // liste les collisions futures d'un objet
    collisionList *getObjectCollisionList(int objId,
                                          collisionList *listToAppend = NULL);

    bool checkGoal(int id);

    // bouge tous les objets pour la durée donnée (en s)
    void moveAllObj(double time);

    void writePlayers();

    void setPlayer(int id, vector pos, vector speed, double orientation,
                   double size = PLAYER_SIZE, double mass = PLAYER_MASS);

    void print();

    void aller_chercher_du_pain(int n);
};

gameInformations play_match(Chromosome *c1, Chromosome *c2, bool save = false);
