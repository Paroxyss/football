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

    g.players[0].x = 900;
    g.players[0].y = 400;

    g.ball.x = 1000;
    g.ball.y = 500;

    while (true) {
        g.print();
        g.ball.x += 10;
		g.players[0].x += 5;
		g.players[0].y -= 2.5;
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
