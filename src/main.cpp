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
#include <string>
#include <thread>
#include <unistd.h>

void generation(Population *pop, gameStatistics *res) {
    auto infos = pop->next();
    res->totalCollisions = infos.totalCollisions;
    res->totalGoals = infos.totalGoals;
    res->collisionsMean = infos.collisionsMean;
    res->goalsMean = infos.goalsMean;
    res->scoreMean = infos.scoreMean;
}

int main() {
    srand(time(NULL));

    const int n = POPULATION_SIZE / NB_THREAD;
    int gen = 0;

    Population **pops = new Population *[NB_THREAD];
    std::thread *threads[NB_THREAD];
    gameStatistics infos[NB_THREAD];

    for (int i = 0; i < NB_THREAD; i++) {
        pops[i] = new Population(n);
    }

    while (gen < N) {
        gameStatistics totalInfos = {.totalCollisions = 0,
                                     .totalGoals = 0,
                                     .scoreMean = 0,
                                     .goalsMean = 0,
                                     .collisionsMean = 0};
        for (int i = 0; i < NB_THREAD; i++) {
            threads[i] = new std::thread(generation, pops[i], &infos[i]);
        }

        for (auto &thread : threads) {
            thread->join();
        }

        for (int i = 0; i < NB_THREAD; i++) {
            totalInfos.totalCollisions += infos[i].totalCollisions;
            totalInfos.totalGoals += infos[i].totalGoals;
            totalInfos.collisionsMean += infos[i].collisionsMean / NB_THREAD;
            totalInfos.goalsMean += infos[i].goalsMean / NB_THREAD;
            totalInfos.scoreMean += infos[i].scoreMean / NB_THREAD;
        }

        shufflePopulations(pops, NB_THREAD);
        gen += NB_THREAD;

        std::cout << "Stats gen " << gen << " " << totalInfos << std::endl;
    }

    Population *p = joinPopulation(pops, NB_THREAD);
    auto meilleurs = p->tournament(p->size, false);

    play_match(std::get<0>(meilleurs), std::get<1>(meilleurs), true);

    for (int i = 0; i < NB_THREAD; i++) {
        delete pops[i];
        delete threads[i];
    }
    delete[] pops;

    delete p;
    _exit(EXIT_SUCCESS);
}
