#include <iostream>
#include <math.h>
#include <ostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "Display.h"
#include "Game.hpp"
#include "config.h"
#include "stdlib.h"

Game::Game(int playerNumber) {
    this->playerNumber = playerNumber;
    this->players = (player *)malloc(playerNumber * sizeof(player));

    ball = {.x = 0, .y = 0, .vx = 0, .vy = 0, .size = BALL_SIZE};
    for (int i = 0; i < playerNumber; i++) {
        this->players[i].x = 0;
        this->players[i].y = 0;
        this->players[i].orientation = 0;
        this->players[i].acceleration = 0;
        this->players[i].size = PLAYER_SIZE;
    };
}

Game::~Game() { free(players); }

void Game::tick() {
    ball.x += ball.vx * GAME_RESOLUTION;
    ball.y += ball.vy * GAME_RESOLUTION;

    if (ball.x + BALL_SIZE > MAP_LENGTH && ball.vx > 0) {
        ball.vx = -ball.vx;
    } else if (ball.x - BALL_SIZE < 0 && ball.vx < 0) {
        ball.vx = -ball.vx;
    } else if (ball.y + BALL_SIZE > MAP_HEIGHT && ball.vy > 0) {
        ball.vy = -ball.vy;
    } else if (ball.y - BALL_SIZE < 0 && ball.vy < 0) {
        ball.vy = -ball.vy;
    }
    ball.vx *= 0.90;
    ball.vy *= 0.90;

    for (int i = 0; i < playerNumber; i++) {
        if (players[i].acceleration > 0) {
			players[i].vx += players[i].acceleration * cos(players[i].orientation);
			players[i].vy += players[i].acceleration * sin(players[i].orientation);
			players[i].acceleration = 0;
        }

        players[i].x += players[i].vx * GAME_RESOLUTION;
        players[i].y += players[i].vy * GAME_RESOLUTION;
		
        if (players[i].x + players[i].size > MAP_LENGTH && players[i].vx > 0) {
            players[i].vx = -players[i].vx;
        } else if (players[i].x - players[i].size < 0 && players[i].vx < 0) {
            players[i].vx = -players[i].vx;
        } else if (players[i].y + players[i].size > MAP_HEIGHT && players[i].vy > 0) {
            players[i].vy = -players[i].vy;
        } else if (players[i].y - players[i].size < 0 && players[i].vy < 0) {
            players[i].vy = -players[i].vy;
        }
		
        players[i].vx *= 0.90;
        players[i].vy *= 0.90;
    }
};

void Game::doAction(unsigned int id, double rotation, double acceleration){

};

void Game::setBall(double x, double y, double vx, double vy, double size){

};

void Game::setPlayer(double x, double y, double speed, double orientation, double size){

};

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
    d.drawCircle('.', ball.x, ball.y, ball.size, TERM_YELLOW);

    for (int i = 0; i < playerNumber; i++) {
        d.drawCircle('X', players[i].x, players[i].y, players[i].size, TERM_BLUE);
        d.drawLine('-', players[i].x, players[i].y, players[i].x + cos(players[i].orientation) * players[i].size,
                   players[i].y + sin(players[i].orientation) * players[i].size);
    }

    d.cursorToBottom();
};
