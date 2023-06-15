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
    auto g = Game(6);

    g.ball.pos = {.x = MAP_LENGTH / 2., .y = MAP_HEIGHT / 2.};
    g.ball.vitesse = {.x = 3, .y = 3};

    int c[] = {2, 1};
    g.set_players(c, 2);

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

        for (int i = 0; i < 8; i++) {
            g.tick();
        }

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
