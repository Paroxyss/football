#include <cmath>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <math.h>
#include <ostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "Game.hpp"
#include "Matrix.h"
#include "Vector.hpp"
#include "config.h"
#include "stdlib.h"
#include "util.hpp"

std::ofstream csvOutputFile;

#define SETWALL(id, px, py, dx, dy)                                            \
    this->walls[id].pos.x = px;                                                \
    this->walls[id].pos.y = py;                                                \
    this->walls[id].vitesse.x = dx;                                            \
    this->walls[id].vitesse.y = dy;

#define SETGOAL(id, px, py, dx, dy, s)                                         \
    this->goals[id].pos.x = px;                                                \
    this->goals[id].pos.y = py;                                                \
    this->goals[id].vitesse.x = dx;                                            \
    this->goals[id].vitesse.y = dy;                                            \
    this->goals[id].size = s;

Game::Game(int playerNumber, bool logToFile) {
    this->logToFile = logToFile;
    if (this->logToFile) {
        csvOutputFile.open("game.csv");
        csvOutputFile << "STARTGAME, " << playerNumber / 2 << "," << MAP_HEIGHT
                      << "," << MAP_LENGTH << "," << BALL_SIZE << ","
                      << PLAYER_SIZE << std::endl;
    }
    this->playerNumber = playerNumber;
    this->players = new player[playerNumber];

    this->wallNumber = 4;
    this->walls = new wall[this->wallNumber];

    this->goals = new wall[2];

    SETWALL(0, 0, 0, 1, 0);
    SETWALL(1, 0, 0, 0, 1);
    SETWALL(2, MAP_LENGTH, MAP_HEIGHT, 1, 0);
    SETWALL(3, MAP_LENGTH, MAP_HEIGHT, 0, 1);

    SETGOAL(0, 0,
            static_cast<float>(MAP_HEIGHT) / 2 -
                static_cast<float>(GOAL_HEIGHT) / 2,
            0, 1, GOAL_HEIGHT);
    SETGOAL(1, MAP_LENGTH,
            static_cast<float>(MAP_HEIGHT) / 2 -
                static_cast<float>(GOAL_HEIGHT) / 2,
            0, 1, GOAL_HEIGHT);

    setBall({.x = 0, .y = 0}, {.x = 0, .y = 0}, BALL_SIZE);
    for (int i = 0; i < playerNumber; i++) {
        setPlayer(i, {.x = 0, .y = 0}, {.x = 0, .y = 0}, 0, PLAYER_SIZE);
        players[i].inputs = NULL;
    }
}

Game::~Game() {

    for (int i = 0; i < this->playerNumber; i++) {
        delete this->players[i].inputs;
    }

    delete[] players;
    delete[] walls;
    delete[] goals;
}

/*
    pour 3 joueurs, on peut par exemple avoir {2, 1}

    Si on place l'équipe de droite les joueurs sont placés dans l'ordre inverse
    d'apparition dans la configuration.
*/
void Game::set_players(const int conf[], int n) {
    int c = 0, s = 0;
    double spx = MAP_LENGTH / 2. / (double)(n + 1);

    for (int i = 0; i < n; i++) {
        s += conf[i];
    }

    if (2 * s != this->playerNumber)
        throw std::invalid_argument("invalid conf");

    for (int i = 0; i < n; i++) {
        double spy = MAP_HEIGHT / (double)(conf[i] + 1);

        for (int k = 1; k <= conf[i]; k++) {
            this->players[c].pos = {.x = (i + 1) * spx, .y = k * spy};
            this->players[c].vitesse = {.x = 0, .y = 0};
            this->players[c].orientation = randomDouble() * M_PI;

            c++;
        }
    }

    for (int i = 0; i < n; i++) {
        double spy = MAP_HEIGHT / (double)(conf[i] + 1);

        for (int k = conf[i]; k >= 1; k--) {
            this->players[c].pos = {.x = MAP_HEIGHT / 2. + spx * (n - i + 1),
                                    .y = k * spy};
            this->players[c].vitesse = {.x = 0, .y = 0};

            // on donne aux joueurs de droite le même angle que ceux de gauche,
            // c'est après en python que l'on ajoute M_PI pour donner la
            // symétrie.
            this->players[c].orientation = this->players[c - s].orientation;

            c++;
        }
    }
}

inline double distancecarre(ball &p, const ball &b) {
    return normeCarre(p.pos - b.pos);
}

inline collisionList *insert(collisionList *list, ball *actor, ball *secondary,
                             CollisionType type, double time = INFINITY) {
    collisionList *l = new collisionList;
    l->actor = actor;
    l->secondary = secondary;
    l->time = time;
    l->type = type;
    l->next = list;
    return l;
}
double getWallCollisionTime(ball *obj, ball *wall) {
    vector MO = obj->pos - wall->pos;

    vector n = {
        .x = -wall->vitesse.y,
        .y = wall->vitesse.x,
    };

    double c = abs(dotProduct(MO, n) / norme(n));
    double ev = abs(dotProduct(obj->vitesse, n) / norme(n));
    double T = c - obj->size;

    return T / ev;
}

double getTwoBallCollisionTime(ball *b1, ball *b2) {
    vector v_relative = b1->vitesse - b2->vitesse;
    vector LM = b1->pos - b2->pos;
    double d = b1->size + b2->size;

    vector n = {
        .x = -v_relative.y,
        .y = v_relative.x,
    };

    double c = abs(dotProduct(LM, n) / norme(n));
    double ev = abs(dotProduct(LM, v_relative) / norme(v_relative));
    double pprime = sqrt(pow(d, 2) - pow(c, 2));

    return (ev - pprime) / norme(v_relative);
}

collisionList *Game::getObjectCollisionList(int objId,
                                            collisionList *listToAppend) {
    struct ball *selected;
    if (objId == -1) {
        selected = &ball;
    } else {
        selected = &(this->players[objId]);
    }

    for (int i = 0; i < wallNumber; i++) {
        wall &w = this->walls[i];
        // on regarde la distance au mur, si elle est inférieure au rayon de
        // la balle, c'est que la balle est en collision
        vector normal = {.x = -w.vitesse.y, .y = w.vitesse.x};
        double d = dotProduct(selected->pos - w.pos, normal) / norme(normal);
        if (abs(d) <
            selected->size) { // la balle est trop près du mur, il y a collision
            listToAppend = insert(listToAppend, selected, &w, WALL);
        }
    }

    // test de collisions triangulaire
    for (int i = objId + 1; i < playerNumber; i++) {
        if (distancecarre(*selected, this->players[i]) <
            pow(selected->size + PLAYER_SIZE, 2)) {
            listToAppend =
                insert(listToAppend, selected, &this->players[i], CIRCLE);
        }
    }

    // la balle va tester ses propres collisions avec tout le monde.
    /*if (objId != -1 &&
        distancecarre(*selected, ball) < pow(PLAYER_SIZE + BALL_SIZE, 2)) {
        c = insert(c, selected, &ball, CIRCLE);
    }*/

    return listToAppend;
}

void timeCollisionList(collisionList *collision) {
    if (!collision) {
        return;
    }
    if (collision->type == WALL) {
        collision->time =
            getWallCollisionTime(collision->actor, collision->secondary);
    } else {
        collision->time =
            getTwoBallCollisionTime(collision->actor, collision->secondary);
    }
    timeCollisionList(collision->next);
}

void computeCollisionCircle(ball *obj1, ball *obj2) {
    vector x1 = obj1->pos;
    double &m1 = obj1->mass;
    vector &v1 = obj1->vitesse;

    vector x2 = obj2->pos;
    vector &v2 = obj2->vitesse;
    double &m2 = obj2->mass;

    auto dv1 = (x1 - x2) *
               (dotProduct(v1 - v2, x1 - x2) / normeCarre(x1 - x2)) *
               ((double)(2 * m2) / (m1 + m2));
    auto dv2 = (x2 - x1) *
               (dotProduct(v2 - v1, x2 - x1) / normeCarre(x2 - x1)) *
               ((double)(2 * m1) / (m1 + m2));

    v1 = v1 - dv1;
    v2 = v2 - dv2;
}

void computeCollisionWall(ball &b, ball *w) {
    vector um = w->vitesse / norme(w->vitesse);
    vector normal_um = {.x = um.y, .y = -um.x};

    b.vitesse = dotProduct(-b.vitesse, normal_um) * normal_um +
                dotProduct(b.vitesse, um) * um;
}

collisionList *findFirstCollision(collisionList *list) {
    if (!list)
        return NULL;

    switch (list->type) {
    case CIRCLE:
        list->time = getTwoBallCollisionTime(list->actor, list->secondary);
        break;
    case WALL:
        list->time = getWallCollisionTime(list->actor, list->secondary);
        break;
    }

    auto nextBest = findFirstCollision(list->next);
    if (nextBest && nextBest->time < list->time) {
        return nextBest;
    }
    return list;
}

void freeCollisionList(collisionList *list) {
    if (list == NULL) {
        return;
    }
    freeCollisionList(list->next);
    delete[] list;
}

void Game::moveAllObj(double percent) {
    ball.pos += ball.vitesse * percent;
    for (int i = 0; i < playerNumber; i++) {
        players[i].pos += players[i].vitesse * percent;
    }
}

void Game::tick(double timeToAdvance, bool root) {
    if (root) {

        if (logToFile) {
            csvOutputFile << "1" << std::endl;
        }

        ball.vitesse -= norme(ball.vitesse) * ball.vitesse / 200;
        for (int i = 0; i < playerNumber; i++) {
            players[i].vitesse -=
                norme(players[i].vitesse) * players[i].vitesse / 40;
        }
    }
    // on fait tout avancer
    moveAllObj(timeToAdvance);

    collisionList *c = NULL;

    for (int i = 0; i < playerNumber + 1; i++) {
        c = getObjectCollisionList(i - 1, c);
    }
    if (c != NULL) {
        moveAllObj(-timeToAdvance);
        this->infos.collisions += 1;

        collisionList *firstCollision = findFirstCollision(c);

        moveAllObj(firstCollision->time);

        switch (firstCollision->type) {
        case CIRCLE:
            computeCollisionCircle(firstCollision->actor,
                                   firstCollision->secondary);
            break;
        case WALL:
            computeCollisionWall(*firstCollision->actor,
                                 firstCollision->secondary);
        }
        tick(timeToAdvance - firstCollision->time, false);
    }
    if (root && logToFile) {
        writePlayers();
    }
    freeCollisionList(c);
};

void Game::writePlayers() {
    csvOutputFile << "2," << (double)ball.pos.x << "," << (double)ball.pos.y
                  << ",";
    for (int i = 0; i < playerNumber; i++) {
        csvOutputFile << (double)this->players[i].orientation << ",";
        for (int j = 0; j < this->players[i].inputs->ligne; j++) {
            csvOutputFile << (double)(this->players[i].inputs->get(j, 0))
                          << ",";
        }
    }
    csvOutputFile << std::endl;
}

void Game::doAction(unsigned int id, double rotation, double acceleration) {
    players[id].orientation += rotation;
    players[id].vitesse.x += acceleration * cos(players[id].orientation);
    players[id].vitesse.y += acceleration * sin(players[id].orientation);
    players[id].acceleration = 0;
};

void Game::setBall(vector pos, vector vitesse, double size, double mass) {
    this->ball = {.pos = pos, .vitesse = vitesse, .size = size, .mass = mass};
}

void Game::setPlayer(int id, vector pos, vector speed, double orientation,
                     double size, double mass) {
    this->players[id].pos = pos;
    this->players[id].vitesse = speed;
    this->players[id].orientation = orientation;
    this->players[id].size = size;
    this->players[id].acceleration = 0;
    this->players[id].mass = mass;
}

bool Game::checkGoal(int id) {
    double cage = getWallCollisionTime(&this->ball, &this->goals[id]);

    if (cage < 0 || cage > 1)
        return false;

    auto dist = this->ball.pos - goals[id].pos;

    auto vertical = dotProduct(this->goals[id].vitesse, dist) /
                    norme(this->goals[id].vitesse);

    if (0 < vertical && vertical < GOAL_HEIGHT) {
        return true;
    }

    return false;
}

/*
    La valeur de g.infos.score est positive lorsque c1 remporte la partie,
    négative dans le cas contraire. c1 est placé à gauche du terrain tandis que
    c2 à droite.
*/
gameInformations play_match(Chromosome *c1, Chromosome *c2, bool save) {
    auto g = Game(2 * EQUIPE_SIZE, save);
    g.ball.pos = {.x = MAP_LENGTH / 2., .y = MAP_HEIGHT / 2.};
    g.ball.vitesse = {.x = 0, .y = 0};

    int c[] = {1};
    g.set_players(c, 1);

    for (int k = 0; k < GAME_DURATION; k++) {

        auto r1 = c1->collect_and_apply(g.players, g.players + EQUIPE_SIZE,
                                        &g.ball,  false);
        auto r2 = c2->collect_and_apply(g.players + EQUIPE_SIZE, g.players,
                                        &g.ball,  true);

        for (int a = 0; a < 2; a++) {
            for (int i = 0; i < EQUIPE_SIZE; i++) {
                Matrix *r = (a == 0) ? r1 : r2;

                double rotation = r->get(0, i) / 10;
                double acceleration = r->get(1, i);

                if (acceleration < 0) {
                    acceleration /= 5;
                }
                g.doAction(i + a * EQUIPE_SIZE, rotation, acceleration);
            }
        }

        delete r1;
        delete r2;

        g.tick();

        bool bc1 = g.checkGoal(0);
        bool bc2 = g.checkGoal(1);
        if (bc1 || bc2) {
            g.infos.score += bc1 ? -1 : 1;
            g.infos.goals += 1;

            g.ball.pos.x = (float)MAP_LENGTH / 2;
            g.ball.pos.y = (float)MAP_HEIGHT / 2;
            g.ball.vitesse.x = 0;
            g.ball.vitesse.y = 0;

            g.set_players(c, 1);
        }
    }

    if (save) {
        csvOutputFile.close();
    }

    if (g.infos.score == 0) {
        g.infos.score = likelyness(0.5) ? 1 : -1;
    }

    return g.infos;
};
