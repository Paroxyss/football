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
    auto g = Game(4);

    g.players[0].pos = {.x = 300, .y = 500};
    g.players[0].vitesse = {.x = 50, .y = 60};
    g.players[1].pos = {.x = 600, .y = 500};
    g.players[1].vitesse = {.x = 40, .y = 60};
    g.players[2].pos = {.x = 900, .y = 500};
    g.players[2].vitesse = {.x = 50, .y = 60};
    g.players[3].pos = {.x = 1200, .y = 500};
    g.players[3].vitesse = {.x = 40, .y = 60};

    g.ball.pos = {.x = 600, .y = 900};
    g.ball.vitesse = {.x = 1, .y = -3};

    int i = 0;
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    auto total =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    auto peak = total;
    total = 0;
    peak = 0;

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        g.tick();
        auto end = std::chrono::high_resolution_clock::now();

        auto actual =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start)
                .count();
		total += actual;
        if (actual > peak)
            peak = total;
        i++;

        g.print();
        std::cout << "moyenne : " << total / i << " us | "
                  << "peak : " << peak << " us" << std::endl;

        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
