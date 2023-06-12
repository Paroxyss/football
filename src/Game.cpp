#include <iostream>
#include <math.h>
#include <ostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "Display.h"
#include "Game.hpp"
#include "Vector.hpp"
#include "config.h"
#include "stdlib.h"

Game::Game(int playerNumber) {
    this->playerNumber = playerNumber;
    this->players = (player *)malloc(playerNumber * sizeof(player));

    setBall({.x = 0, .y = 0});
    for (int i = 0; i < playerNumber; i++) {
        setPlayer(i, {.x = 0, .y = 0}, {.x = 0, .y = 0}, 0, PLAYER_SIZE);
    };
}

Game::~Game() {
    free(players);
}

inline double distancecarre(player &p, ball &b) {
    return normeCarre(p.pos - b.pos);
}
inline double distancecarre(player &p, player &b) {
    return normeCarre(p.pos - b.pos);
}

void computeCollision(vector x1, vector &v1, double m1, vector x2, vector &v2,
                      double m2) {

    auto dv1 = (x1 - x2) *
               (dotProduct(v1 - v2, x1 - x2) / normeCarre(x1 - x2)) *
               ((double)(2 * m2) / (m1 + m2));
    auto dv2 = (x2 - x1) *
               (dotProduct(v2 - v1, x2 - x1) / normeCarre(x2 - x1)) *
               ((double)(2 * m1) / (m1 + m2));

    v1 = v1 - dv1;
    v2 = v2 - dv2;
}

void Game::tick() {
    ball.pos += ball.vitesse * GAME_RESOLUTION;

    if (ball.pos.x + BALL_SIZE > MAP_LENGTH && ball.vitesse.x > 0) {
        ball.vitesse.x = -ball.vitesse.x;
    } else if (ball.pos.x - BALL_SIZE < 0 && ball.vitesse.x < 0) {
        ball.vitesse.x = -ball.vitesse.x;
    } else if (ball.pos.y + BALL_SIZE > MAP_HEIGHT && ball.vitesse.y > 0) {
        ball.vitesse.y = -ball.vitesse.y;
    } else if (ball.pos.y - BALL_SIZE < 0 && ball.vitesse.y < 0) {
        ball.vitesse.y = -ball.vitesse.y;
    }
    ball.vitesse += -ball.vitesse / BALL_MASS;

    for (int i = 0; i < playerNumber; i++) {
        // std::cout << "computing player " << i << " of " << playerNumber <<
        // std::endl;
        player &p = players[i];
        if (p.acceleration > 0) {
            p.vitesse.x += p.acceleration * cos(p.orientation);
            p.vitesse.y += p.acceleration * sin(p.orientation);
            p.acceleration = 0;
        }

        p.pos += p.vitesse * GAME_RESOLUTION;

        if (p.pos.x + p.size > MAP_LENGTH && p.vitesse.x > 0) {
            p.vitesse.x = -p.vitesse.x;
        } else if (p.pos.x - p.size < 0 && p.vitesse.x < 0) {
            p.vitesse.x = -p.vitesse.x;
        } else if (p.pos.y + p.size > MAP_HEIGHT && p.vitesse.y > 0) {
            p.vitesse.y = -p.vitesse.y;
        } else if (p.pos.y - p.size < 0 && p.vitesse.y < 0) {
            p.vitesse.y = -p.vitesse.y;
        }

        p.vitesse += -p.vitesse / PLAYER_MASS;

        if (distancecarre(p, ball) < pow(PLAYER_SIZE + BALL_SIZE, 2)) {
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
        }
    }
};

void Game::doAction(unsigned int id, double rotation, double acceleration){

};

void Game::setBall(vector pos, vector vitesse, double size) {
    this->ball = {.pos = pos, .vitesse = vitesse, .size = size};
}

void Game::setPlayer(int id, vector pos, vector speed, double orientation,
                     double size) {
    this->players[id].pos = pos;
    this->players[id].vitesse = speed;
    this->players[id].orientation = orientation;
    this->players[id].size = size;
    this->players[id].acceleration = 0;
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
    d.drawCircle('.', ball.pos.x, ball.pos.y, BALL_SIZE, TERM_YELLOW);

    for (int i = 0; i < playerNumber; i++) {
        player &p = players[i];
        d.drawCircle('X', p.pos.x, p.pos.y, PLAYER_SIZE, TERM_BLUE);
        d.drawLine('-', p.pos.x, p.pos.y, p.pos.x + cos(p.orientation) * p.size,
                   p.pos.y + sin(p.orientation) * p.size);
    }

    d.cursorToBottom();
};
