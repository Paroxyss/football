#include "Chromosome.hpp"
#include "Game.hpp"
#include "Matrix.h"
#include "Population.hpp"
#include "config.h"
#include "util.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <thread>
#include <unistd.h>

int main() {
    std::cout << randomDouble() << std::endl;
    int i = 0;
    srand(time(NULL));
    while (1) {
        int seed = time(NULL) % 1000 + 1000 * i;

        auto c1 = Chromosome();
        c1.randomize();
        auto c2 = Chromosome();
        c2.randomize();

        srand(seed);
        std::cout << "starting game" << std::endl;
        int score = Game::play_match(&c1, &c2, false);
        std::cout << "score : " << score << std::endl;

        if (abs(score) > 1) {
            srand(seed);
            score = Game::play_match(&c1, &c2, true);
            exit(0);
        }
        i++;
    }
}
