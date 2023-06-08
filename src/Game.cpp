#include <iostream>
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
        this->players[i].x           = 0;
        this->players[i].y           = 0;
        this->players[i].orientation = 0;
        this->players[i].vitesse     = 0;
        this->players[i].size        = PLAYER_SIZE;
    };
}

Game::~Game() { free(players); }

void Game::tick(){

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
    d.drawBorders(TERM_WHITE);

    d.drawCircle('.', ball.x, ball.y, ball.size, TERM_YELLOW);

    for (int i = 0; i < playerNumber; i++) {
        d.drawCircle('X', players[i].x, players[i].y, players[i].size, TERM_BLUE);
    }

    d.cursorToBottom();
};
