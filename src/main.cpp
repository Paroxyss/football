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
    srand(time(NULL));

    const int n = POPULATION_SIZE / NB_THREAD;
    int gen = 0;

    Population **pops = new Population *[NB_THREAD];
    std::thread *threads[NB_THREAD];

    for (int i = 0; i < NB_THREAD; i++) {
        pops[i] = new Population(n);
    }

    while (gen < N) {
        std::cout << "starting generation " << gen << std::endl;

        for (int i = 0; i < NB_THREAD; i++) {
            threads[i] = new std::thread(generation, pops[i]);
        }

        for (auto &thread : threads) {
            thread->join();
        }

        shufflePopulations(pops, NB_THREAD);
        gen += NB_THREAD;
    }

    Population *p = joinPopulation(pops, NB_THREAD);
    auto meilleurs = p->tournament(p->size, false);

    play_match(meilleurs.first, meilleurs.second, true);

    for (int i = 0; i < NB_THREAD; i++) {
        delete pops[i];
        delete threads[i];
    }
    delete[] pops;

    delete p;
    _exit(EXIT_SUCCESS);
}
