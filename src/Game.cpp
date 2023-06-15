#include <cmath>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <math.h>
#include <ostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

#include "Display.h"
#include "Game.hpp"
#include "Vector.hpp"
#include "config.h"
#include "stdlib.h"
#include "util.hpp"

std::ofstream csvOutputFile;

#ifdef LOGGAME
#define writeToLogFile(qqch) csvOutputFile << qqch << std::endl;
#else
#define writeToLogFile(eventType) ;
#endif

#define SETWALL(id, px, py, dx, dy)                                            \
    this->walls[id].pos.x = px;                                                \
    this->walls[id].pos.y = py;                                                \
    this->walls[id].vitesse.x = dx;                                            \
    this->walls[id].vitesse.y = dy;

Game::Game(int playerNumber) {
#ifdef LOGGAME
    csvOutputFile.open("game.csv");
	csvOutputFile << "STARTGAME, " << playerNumber / 2 << ", " << MAP_HEIGHT << ", " << MAP_LENGTH << ", " << BALL_SIZE << ", " << PLAYER_SIZE << std::endl;
# endif
	
    this->playerNumber = playerNumber;
    this->players = (player *)malloc(playerNumber * sizeof(player));

    this->wallNumber = 4;
    this->walls = (wall *)malloc(this->wallNumber * sizeof(wall));

    SETWALL(0, 0, 0, 1, 0)
    SETWALL(1, 0, 0, 0, 1)
    SETWALL(2, MAP_LENGTH, MAP_HEIGHT, 1, 0)
    SETWALL(3, MAP_LENGTH, MAP_HEIGHT, 0, 1)

    setBall({.x = 0, .y = 0}, {.x = 0, .y = 0}, BALL_SIZE);
    for (int i = 0; i < playerNumber; i++) {
        setPlayer(i, {.x = 0, .y = 0}, {.x = 0, .y = 0}, 0, PLAYER_SIZE);
    };
}

Game::~Game() {
    free(players);
    free(walls);
}

/*
    pour 3 joueurs, on peut par exemple avoir {2, 1}

    Si on place l'équipe de droite les joueurs sont placés dans l'ordre inverse
    d'apparition dans la configuration.
*/
void Game::set_players(int conf[], int n) {
    int c = 0, s = 0, t = true;
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
            this->players[c++].orientation = randomDouble() * M_PI;
        }
    }

    for (int i = n - 1; i >= 0; i--) {
        double spy = MAP_HEIGHT / (double)(conf[i] + 1);
        for (int k = 1; k <= conf[i]; k++) {
            this->players[c].pos = {.x = MAP_LENGTH - (i + 1) * spx,
                                    .y = k * spy};
            this->players[c].orientation =
                this->players[this->playerNumber - (c + 1)].orientation - M_PI;
            c++;
        }
    }
}

inline double distancecarre(ball &p, ball &b) {
    return normeCarre(p.pos - b.pos);
}

inline collisionList *insert(collisionList *list, ball *actor, ball *secondary,
                             CollisionType type, double time = INFINITY) {
    collisionList *l = (collisionList *)malloc(sizeof(collisionList));
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
    free(list);
}

void printCollisionList(collisionList *list) {
    if (list == NULL) {
        std::cout << "Fin de liste" << std::endl;
        return;
    }

    if (list->type == CIRCLE) {
        std::cout << "Collision à " << list->time << " de " << list->actor->pos
                  << " avec la sphere " << list->secondary->pos << std::endl;
    } else {
        std::cout << "Collision à " << list->time << " de " << list->actor->pos
                  << " avec un mur" << std::endl;
    }
    printCollisionList(list->next);
}

void Game::tick(double timeToAdvance, bool root) {
    if (root) {
        writeToLogFile("TICK");
    }
    // on fait tout avancer
    ball.pos += ball.vitesse * timeToAdvance;
    for (int i = 0; i < playerNumber; i++) {
        players[i].pos += players[i].vitesse * timeToAdvance;
    }

    collisionList *c = NULL;

    for (int i = 0; i < playerNumber + 1; i++) {
        c = getObjectCollisionList(i - 1, c);
    }
    if (c != NULL) {
        ball.pos -= ball.vitesse * timeToAdvance;

        for (int i = 0; i < playerNumber; i++) {
            players[i].pos -= players[i].vitesse * timeToAdvance;
        }

        collisionList *firstCollision = findFirstCollision(c);

        ball.pos += ball.vitesse * firstCollision->time;

        for (int i = 0; i < playerNumber; i++) {
            players[i].pos += players[i].vitesse * firstCollision->time;
        }
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
#ifdef LOGGAME
    if (root) {
        writePlayers();
    }
#endif
    freeCollisionList(c);
};

#ifdef LOGGAME
void Game::writePlayers() {
    csvOutputFile << "ALLSETPOS";
    csvOutputFile << ", " << ball.pos.x << ", " << ball.pos.y;
    for (int i = 0; i < playerNumber; i++) {
        csvOutputFile << ", " << players[i].pos.x << ", " << players[i].pos.y;
    }
    csvOutputFile << std::endl;
}
#endif

void Game::doAction(unsigned int id, double rotation, double acceleration){

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

void moveCursor(unsigned int x, unsigned int y) {
    printf("\033[%d;%df", y, x);
}

void clearScreen() {
    std::cout << "\033[2J" << std::endl;
}

void setPixel(char c, unsigned int x, unsigned int y) {
    moveCursor(x, y);
    std::cout << c;
}

void Game::print() { // affichage provisoire de la partie, sur le terminal
                     // On obtient la taille du terminal
    auto d = Display(MAP_LENGTH, MAP_HEIGHT);

    d.clear();
    d.drawCircle('.', ball.pos.x, ball.pos.y, ball.size, TERM_YELLOW);
    d.drawLine('=', ball.pos.x, ball.pos.y, ball.pos.x + ball.vitesse.x,
               ball.pos.y + ball.vitesse.y, TERM_RED);

    for (int i = 0; i < playerNumber; i++) {
        player &p = players[i];
        d.drawCircle('X', p.pos.x, p.pos.y, PLAYER_SIZE, TERM_BLUE);
        d.drawLine('-', p.pos.x, p.pos.y, p.pos.x + cos(p.orientation) * p.size,
                   p.pos.y + sin(p.orientation) * p.size);
        d.drawLine('=', p.pos.x, p.pos.y, p.pos.x + p.vitesse.x,
                   p.pos.y + p.vitesse.y, TERM_RED);
    }

    d.cursorToBottom();
};
