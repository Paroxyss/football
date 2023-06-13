#include <cmath>
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

#define SETWALL(id, px, py, dx, dy)                                            \
    this->walls[id].pos.x = px;                                                \
    this->walls[id].pos.y = py;                                                \
    this->walls[id].vitesse.x = dx;                                            \
    this->walls[id].vitesse.y = dy;

Game::Game(int playerNumber) {
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
}

inline double distancecarre(ball &p, ball &b) {
    return normeCarre(p.pos - b.pos);
}

inline collisionList *insert(collisionList *list, ball *obj,
                             CollisionType type) {
    collisionList *l = (collisionList *)malloc(sizeof(collisionList));
    l->obj = obj;
    l->type = type;
    l->next = list;
    return l;
}

// fonction qui liste toutes les collisions en cours, pour pouvoir savoir
// laquelle est arrivé en premier
collisionList *Game::getCollisionList(int objId) {
    struct ball *selected;
    if (objId == -1) {
        std::cout << "C'est la balle" << std::endl;
        selected = &ball;
    } else {
        std::cout << "C'est le joueur " << objId << std::endl;
        selected = &(this->players[objId]);
    }
    collisionList *c = NULL;

    for (int i = 0; i < wallNumber; i++) {
        wall &w = this->walls[i];
        // on regarde la distance au mur, si elle est inférieure au rayon de la
        // balle, c'est que la balle est en collision
        vector normal = {.x = -w.vitesse.y, .y = w.vitesse.x};
        double d = dotProduct(selected->pos - w.pos, normal) / norme(normal);
        if (abs(d) <
            selected->size) { // la balle est trop près du mur, il y a collision
            c = insert(c, &w, WALL);
        }
    }

    for (int i = 0; i < playerNumber; i++) {
        if (i == objId)
            continue;
        if (distancecarre(*selected, this->players[i]) <
            pow(2 * PLAYER_SIZE, 2)) {
            c = insert(c, &this->players[i], CIRCLE);
        }
    }

    if (objId != -1 &&
        distancecarre(*selected, ball) < pow(PLAYER_SIZE + BALL_SIZE, 2)) {
        c = insert(c, &this->ball, CIRCLE);
    }

    return c;
}

// Fonction pour trouver la première collision étant arrivé
backtrackCollisionReport backTrackCollisions(collisionList *collision,
                                             ball &obj) {
    if (collision == NULL) {
        return {.distanceBefore = 0, .obj = NULL};
    }
    double p;
    if (collision->type == CIRCLE) {
        vector d1 = collision->obj->pos - obj.pos;
        double d = collision->obj->size + obj.size;

        std::cout << "DEBUT CIRCLE =========" << std::endl;
        std::cout << "Pos de l'objet " << obj.pos << " vitessse de l'objet "
                  << obj.vitesse << " pos de l'objet collisionné "
                  << collision->obj->pos << std::endl;

        vector vNormal = {.x = -obj.vitesse.y, .y = obj.vitesse.x};
        // p + p'
        double lTotale = abs(dotProduct(d1, obj.vitesse) / norme(obj.vitesse));

        std::cout << "d1: " << d1 << "vNormal: " << vNormal
                  << " vNormal.v = " << dotProduct(vNormal, obj.vitesse)
                  << std::endl;
        std::cout << "d: " << d << std::endl;

        double pprime = sqrt(pow(d, 2) - (pow(dotProduct(vNormal, d1), 2) /
                                          normeCarre(vNormal)));

        std::cout << "membres : " << lTotale << " " << pprime << std::endl;
        p = -abs(lTotale - pprime);
        std::cout << "FIN CIRCLE =========, p: " << p << std::endl;
    }

    if (collision->type == WALL) {
        auto o = collision->obj->pos;
        auto m = collision->obj->vitesse;
        auto a = obj.pos;
        auto v = obj.vitesse;
        auto d = obj.size;

        // calcul en double, on le fait déjà lors du test de collision
        vector normal = {.x = -m.y, .y = m.x};
        double pp = abs(dotProduct(a - o, normal) / norme(normal));
        double pv = abs(dotProduct(v, normal) / norme(normal));

        double r = (d - pp) / pv;

        p = -r * norme(v);
    }

    auto n = backTrackCollisions(collision->next, obj);
    std::cout << "p calculé: " << p << std::endl;
    if (n.distanceBefore <= p)
        return n;
    else
        return {.distanceBefore = p, .obj = collision};
}

void computeCollisionCircle(vector x1, vector &v1, double m1, vector x2,
                            vector &v2, double m2) {

    auto dv1 = (x1 - x2) *
               (dotProduct(v1 - v2, x1 - x2) / normeCarre(x1 - x2)) *
               ((double)(2 * m2) / (m1 + m2));
    auto dv2 = (x2 - x1) *
               (dotProduct(v2 - v1, x2 - x1) / normeCarre(x2 - x1)) *
               ((double)(2 * m1) / (m1 + m2));

    v1 = v1 - dv1;
    v2 = v2 - dv2;
}

void computeCollisionWall(ball *w, ball &b) {
    std::cout << "um" << std::endl;
    vector um = w->vitesse / norme(w->vitesse);
    std::cout << "num" << std::endl;
    vector normal_um = {.x = um.y, .y = -um.x};

    std::cout << "vitesse" << std::endl;
    b.vitesse = dotProduct(-b.vitesse, normal_um) * normal_um +
                dotProduct(b.vitesse, um) * um;
}

void computeCollision(collisionList *collision, ball &b) {
    std::cout << "Entrée de collision" << std::endl;
    std::cout << collision << std::endl;
    std::cout << collision->obj << std::endl;
    if (collision->type == CIRCLE) {
        auto &v1 = b.vitesse;
        auto &v2 = collision->obj->vitesse;
        auto &x1 = b.pos;
        auto &x2 = collision->obj->pos;
        auto m1 = b.mass;
        auto m2 = collision->obj->mass;
        computeCollisionCircle(x1, v1, m1, x2, v2, m2);
    } else {
        computeCollisionWall(collision->obj, b);
    }
}

// fait avancer l'objet n°id, -1 représente la balle.
void Game::tickBall(int objId, double progress) {
    if (progress >= 1)
        return;

    struct ball *selected;
    if (objId == -1) {
        selected = &ball;
    } else {
        selected = &(this->players[objId]);
    }

    selected->pos += selected->vitesse * GAME_RESOLUTION * (1 - progress);

    collisionList *list = this->getCollisionList(objId);
    if (list) {
        auto nv = norme(selected->vitesse);
        // on regarde de combien il faut reculer
        auto res = backTrackCollisions(list, *selected);
        // on recule
        std::cout << "before pos update " << res.distanceBefore << std::endl;

        if (list->type == CIRCLE && false) {
            std::cout << "progress " << progress << std::endl;
            std::cout << selected->pos << " " << selected->vitesse << std::endl;
            fflush(stdout);
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }

        this->players[0].pos += selected->vitesse / nv * res.distanceBefore;
        // on update le progress
        progress = 1 - abs(res.distanceBefore) / nv + progress;
        // on update la collision (update la vitesse des objets impliqués)
        std::cout << "before compute collision " << selected->pos << " "
                  << selected->vitesse << std::endl;
        computeCollision(res.obj, *selected);

        if (list->type == CIRCLE && false) {
            std::cout << "progress " << progress << std::endl;
            std::cout << selected->pos << " " << selected->vitesse << std::endl;
            fflush(stdout);
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }

        tickBall(objId, progress);

        /*
        print();
        std::cout << res.distanceBefore << " " << norme(selected->vitesse)
                  << std::endl;
        std::cout << selected->pos << " " << selected->vitesse << std::endl;
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        */
    }
}

void Game::tick() {
    ball.pos += ball.vitesse * GAME_RESOLUTION;

    /*if (ball.pos.x + BALL_SIZE > MAP_LENGTH && ball.vitesse.x > 0) {
        ball.vitesse.x = -ball.vitesse.x;
    } else if (ball.pos.x - BALL_SIZE < 0 && ball.vitesse.x < 0) {
        ball.vitesse.x = -ball.vitesse.x;
    } else if (ball.pos.y + BALL_SIZE > MAP_HEIGHT && ball.vitesse.y > 0) {
        ball.vitesse.y = -ball.vitesse.y;
    } else if (ball.pos.y - BALL_SIZE < 0 && ball.vitesse.y < 0) {
        ball.vitesse.y = -ball.vitesse.y;
    }*/
    ball.vitesse += -ball.vitesse / BALL_MASS;

    for (int i = 0; i < playerNumber; i++) {
        // std::cout << "computing player " << i << " of " << playerNumber <<
        // std::endl;
        player &p = players[i];
        if (p.acceleration > 0) {
            // on multiplie par la résolution pour compenser le fait que le
            // joueur accélère plus de fois
            p.vitesse.x +=
                p.acceleration * cos(p.orientation) * GAME_RESOLUTION;
            p.vitesse.y +=
                p.acceleration * sin(p.orientation) * GAME_RESOLUTION;
            p.acceleration = 0;
        }

        p.pos += p.vitesse * GAME_RESOLUTION;

        /*if (p.pos.x + p.size > MAP_LENGTH && p.vitesse.x > 0) {
            p.vitesse.x = -p.vitesse.x;
        } else if (p.pos.x - p.size < 0 && p.vitesse.x < 0) {
            p.vitesse.x = -p.vitesse.x;
        } else if (p.pos.y + p.size > MAP_HEIGHT && p.vitesse.y > 0) {
            p.vitesse.y = -p.vitesse.y;
        } else if (p.pos.y - p.size < 0 && p.vitesse.y < 0) {
            p.vitesse.y = -p.vitesse.y;
        }*/

        p.vitesse += -p.vitesse / PLAYER_MASS;

        /*if (distancecarre(p, ball) < pow(PLAYER_SIZE + BALL_SIZE, 2)) {
            auto &v1 = p.vitesse;
            auto &v2 = ball.vitesse;
            auto &x1 = p.pos;
            auto &x2 = ball.pos;
            auto m1 = PLAYER_MASS;
            auto m2 = BALL_MASS;
            computeCollision(x1, v1, m1, x2, v2, m2);
        }
        for (int i2 = i + 1; i2 < playerNumber; i2++) {
            player &p2 = players[i2];
            if (distancecarre(p, p2) < pow(2 * PLAYER_SIZE, 2)) {
                auto &v1 = p.vitesse;
                auto &v2 = p2.vitesse;
                auto &x1 = p.pos;
                auto &x2 = p2.pos;
                auto m1 = PLAYER_MASS;
                auto m2 = PLAYER_MASS;
                computeCollision(x1, v1, m1, x2, v2, m2);
            }
        }*/
    }
};

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

    for (int i = 0; i < playerNumber; i++) {
        player &p = players[i];
        d.drawCircle('X', p.pos.x, p.pos.y, PLAYER_SIZE, TERM_BLUE);
        d.drawLine('-', p.pos.x, p.pos.y, p.pos.x + cos(p.orientation) * p.size,
                   p.pos.y + sin(p.orientation) * p.size);
    }

    d.cursorToBottom();
};
