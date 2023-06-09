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

Game::~Game() { free(players); }

inline double distancecarre(player &p, ball &b) { return normeCarre(p.pos - b.pos); }
inline double distancecarre(player &p, player &b) { return normeCarre(p.pos - b.pos); }

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
    }
};

void Game::doAction(unsigned int id, double rotation, double acceleration){

};

void Game::setBall(vector pos, vector vitesse, double size) {
	this->ball = {
		.pos = pos,
		.vitesse = vitesse,
		.size = size
	};
}

void Game::setPlayer(int id, vector pos, vector speed, double orientation, double size) {
	this->players[id].pos = pos;
	this->players[id].vitesse = speed;
	this->players[id].orientation = orientation;
	this->players[id].size = size;
	this->players[id].acceleration = 0;
	
}

void moveCursor(unsigned int x, unsigned int y) { printf("\033[%d;%df", y, x); }

void clearScreen() { std::cout << "\033[2J" << std::endl; }

void setPixel(char c, unsigned int x, unsigned int y) {
    moveCursor(x, y);
    std::cout << c;
}

void Game::print() { // affichage provisoir de la partie, sur le terminal
                     // On obtient la taille du terminal
    auto d = Display(MAP_LENGTH, MAP_HEIGHT);

    d.clear();
    d.drawCircle('.', ball.pos.x, ball.pos.y, ball.size, TERM_YELLOW);

    for (int i = 0; i < playerNumber; i++) {
		player &p = players[i];
        d.drawCircle('X', p.pos.x, p.pos.y, p.size, TERM_BLUE);
        d.drawLine('-', p.pos.x, p.pos.y, p.pos.x + cos(p.orientation) * p.size,
                   p.pos.y + sin(p.orientation) * p.size);
    }

    d.cursorToBottom();
};
