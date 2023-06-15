#include "Chromosome.hpp"
#include "Game.hpp"
#include "Matrix.h"
#include "Population.hpp"
#include "config.h"
#include "util.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <thread>
#include <unistd.h>

int main() {
    Game g = Game(6);

    g.ball.pos = {.x = MAP_LENGTH / 2., .y = MAP_HEIGHT / 2.};
    g.ball.vitesse = {.x = randomDouble(), .y = randomDouble()};

    int c[] = {2, 1};
    g.set_players(c, 2);

    int i = 0;

    Population *p = new Population(1000);
    Matrix *didier = NULL;

    while (i++ < 500) {
        for (int k = 0; k < 8; k++) {

            g.tick();
        }

        fflush(stdout);
    }
}
