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
    auto g = Game(1);

    g.players[0].pos = {.x = 900, .y = 400};

	g.ball.size = 100;
    g.ball.pos = {.x = 1000, .y = 500};
    g.ball.vitesse = {.x = 100, .y = 100};

    int i = 0;

    while (true) {

        if (i % 10 == 0) {
            g.players[0].orientation += 0.5;
        }

        if (i % 13 == 0) {
            g.players[0].acceleration = 20 + i % 70;
        }

        g.tick();
        g.print();
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i++;
    }
}
