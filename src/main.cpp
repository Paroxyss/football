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

void generation(Population *pop) {
    pop->next();
}

int main() {
    srand(0);

    const int n = 180 / 4;
    int gen = 1;

    Population pops[] = {Population(n), Population(n), Population(n),
                         Population(n)};

    while (gen < 2) {
        std::cout << "starting generation " << gen << std::endl;

        std::thread t1(generation, &pops[0]);
        std::thread t2(generation, &pops[1]);
        std::thread t3(generation, &pops[2]);
        std::thread t4(generation, &pops[3]);

        t1.join();
        t2.join();
        t3.join();
        t4.join();

        shufflePopulations(pops, 4);
        gen += 4;
    }

    Population *p = joinPopulation(pops, 4);
    auto meilleurs = p->tournament(p->size, false);
    play_match(meilleurs.first, meilleurs.second, true);

    delete p;
    _exit(EXIT_SUCCESS);
}
