#include "Chromosome.hpp"
#include "Game.hpp"
#include "Matrix.h"
#include "Network.hpp"
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
    auto g = Game(2);

    g.players[0].pos = {.x = 900, .y = 300};
    g.players[1].pos = {.x = 600, .y = 300};

    g.ball.size = 100;
    g.ball.pos = {.x = 100, .y = 700};
    g.ball.vitesse = {.x = 500, .y = 10};

    int i = 0;

    while (true) {

        for (int i2 = 0; i2 < g.playerNumber; i2++) {
            if (i % (10 + i2) == 0) {
                g.players[i2].orientation += 0.5;
            }

            if (i % (13 + 4 * i2) == 0) {
                g.players[i2].acceleration = 20;
            }
        }

        g.tick();
        g.print();
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i++;
    }
}
