#include <stdexcept>
#include <thread>

#include "Chromosome.hpp"
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

        std::cout << "Stats gen " << gen << " " << totalInfos << " in "
                  << elapsed_seconds.count() << "s" << std::endl;

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
    std::cout << "Opening ... ";
    std::ifstream input;
    input.open(filename);
    if (!input.is_open()) {
        throw std::invalid_argument("File not found");
    }
    std::cout << "Loading ... ";
    Population *p = Population::read(input);
    std::cout << "Ok ! (" << p->size << " chromosomes)" << std::endl
              << "Starting tournament" << std::endl;

    auto meilleurs = p->tournament(p->size, false);

    std::cout << "Saving match" << std::endl;

    play_match(std::get<0>(meilleurs), std::get<1>(meilleurs), true);

    input.close();
    delete p;
}

void play_match(const char *fileC1, const char *fileC2) {
    std::cout << "Opening ... ";
    std::ifstream fc1;
    std::ifstream fc2;
    fc1.open(fileC1);
    fc2.open(fileC2);
    if (!fc1.is_open() || !fc2.is_open()) {
        throw std::invalid_argument("File not found");
    }
    std::cout << "Loading ... ";
    auto c1 = Chromosome::read(fc1);
    auto c2 = Chromosome::read(fc2);
    std::cout << "Ok ! " << std::endl << "Saving match" << std::endl;

    play_match(c1, c2, true);
}

void saveMap(const char *filename, const char *outputFn) {
    std::cout << "Opening ... ";
    std::ifstream input;
    input.open(filename);
    if (!input.is_open()) {
        throw std::invalid_argument("File not found");
    }
    std::cout << "Loading ... ";
    Population *p = Population::read(input);
    std::cout << "Ok ! (" << p->size << " chromosomes)" << std::endl
              << "save Mapping" << std::endl;

    std::ofstream output;
    output.open(outputFn);

    for (int i = 0; i < p->size; i++) {
        auto data = p->pop[i]->get2dProjection();
        output << data.first << ", " << data.second << std::endl;
    }

    input.close();
    output.close();

    delete p;
}

void linearMap(const char **files, const char *outputFn, int fileNumber) {
    std::ofstream output;
    output.open(outputFn);

    for (int i = 0; i < fileNumber; i++) {
        std::cout << "Opening ... ";
        std::ifstream input;
        input.open(files[i]);
        if (!input.is_open()) {
            throw std::invalid_argument("File not found");
        }
		
        std::cout << "Loading ... ";
		std::fflush(stdout);
		
        Population *p = Population::read(input);
        std::cout << "Ok ! (" << p->size << " chromosomes)" << std::endl
                  << "Mapping..." << std::endl;

        for (int i = 0; i < p->size; i++) {
            auto data = p->pop[i]->get2dProjection();
            output << data.first << ", " << data.second << std::endl;
        }

        input.close();

        delete p;
    }
    output.close();
}
