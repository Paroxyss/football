#include <stdexcept>
#include <thread>

#include "Population.hpp"
#include "config.h"
#include "util.hpp"

void generation(Population *pop, gameStatistics *res) {
    auto infos = pop->next();
    res->totalCollisions = infos.totalCollisions;
    res->totalGoals = infos.totalGoals;
    res->collisionsMean = infos.collisionsMean;
    res->goalsMean = infos.goalsMean;
    res->scoreMean = infos.scoreMean;
}

void train(unsigned int generationObjective, unsigned int populationSize,
           unsigned int nbThread) {

    std::cout << "Starting a train of " << generationObjective
              << " generations with " << populationSize << " chromosomes on "
              << nbThread << " threads" << std::endl;

    const int n = populationSize / nbThread;
    int gen = 0;
    int lastSave = 0;

    Population **pops = new Population *[nbThread];
    std::thread *threads[nbThread];
    gameStatistics infos[nbThread];

    for (int i = 0; i < nbThread; i++) {
        pops[i] = new Population(n);
    }

    while (gen < generationObjective) {
        gameStatistics totalInfos = {.totalCollisions = 0,
                                     .totalGoals = 0,
                                     .scoreMean = 0,
                                     .goalsMean = 0,
                                     .collisionsMean = 0};

        auto start = std::chrono::steady_clock::now();

        for (int i = 0; i < nbThread; i++) {
            threads[i] = new std::thread(generation, pops[i], &infos[i]);
        }

        for (auto &thread : threads) {
            thread->join();
        }

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        for (int i = 0; i < nbThread; i++) {
            totalInfos.totalCollisions += infos[i].totalCollisions;
            totalInfos.totalGoals += infos[i].totalGoals;
            totalInfos.collisionsMean += infos[i].collisionsMean / nbThread;
            totalInfos.goalsMean += infos[i].goalsMean / nbThread;
            totalInfos.scoreMean += infos[i].scoreMean / nbThread;
        }

        shufflePopulations(pops, nbThread);
        gen += nbThread;

        std::cout << "Stats gen " << gen << " " << totalInfos << " in " << elapsed_seconds.count() << "s" << std::endl;

        lastSave += nbThread;

        if (lastSave > SAVE_RATE) {
            Population *p = joinPopulation(pops, nbThread);
            std::ofstream out;
            out.open("pops/pop-gen" + std::to_string(gen) + "-at-" +
                     std::to_string(time(NULL)));

            p->write(out);

            out.close();
            delete p;
            lastSave -= SAVE_RATE;
        }
    }

    Population *p = joinPopulation(pops, nbThread);
    std::ofstream out;
    out.open("pops/pop-gen" + std::to_string(gen) + "-at-" +
             std::to_string(time(NULL)));

    p->write(out);

    out.close();

    for (int i = 0; i < nbThread; i++) {
        delete pops[i];
        delete threads[i];
    }
    delete[] pops;
    delete p;
}

void simulateAndSave(const char *filename) {
    std::ifstream input;
    input.open(filename);
    if (!input.is_open()) {
        throw std::invalid_argument("File not found");
    }

    Population *p = Population::read(input);
    auto meilleurs = p->tournament(p->size, false);

    play_match(std::get<0>(meilleurs), std::get<1>(meilleurs), true);

    input.close();
    delete p;
}
