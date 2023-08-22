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
        pops[i]->initialize();
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

        std::cout << "\nStats gen " << gen << " " << totalInfos << " in "
                  << elapsed_seconds.count() << std::endl;

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

/*
    Pour choisir les deux équipes dont le match sera visualisé,
    on organise un tournois en n^2 ce qui maximise l'élitisme.

    Le désavantage étant que ça rallonge considérable le temps d'exécution
    surtout pour des larges population. Il faudrait donc repenser cette étape
    hors tests.
*/
void play_with_nexts(Population *p, int i, int *scores) {
    for (int j = i + 1; j < p->size; j++) {
        auto res = play_match(p->pop[i], p->pop[j], 0);
        int w = res.score > 0 ? i : j;

        scores[w] += abs(res.score);
    }
}

/**
 * @brief On recherche les deux meilleures équipes pour afficher leur match.
 *
 * @param filename Fichier dans lequel récupérer la population.
 *
 * TODO: Cette méthode est en n^2, il faut en implémenter une plus rapide
 * soit avec un Tournament Tree, un QuickSelect ou un Heap-max
 */
void simulateAndSave(const char *filename) {
    std::ifstream input;
    input.open(filename);

    if (!input.is_open())
        throw std::invalid_argument("File not found");

    int n = std::thread::hardware_concurrency();
    std::thread *threads[n];

    Population *p = Population::read(input);

    int *scores = (int *)calloc(p->size, sizeof(int));
    int b1 = INT_MIN, b2 = INT_MIN;
    Chromosome *c1, *c2;

    int c = 0;
    while (c < p->size) {
        int r = p->size - c - n < 0 ? p->size - c : n;

        for (int i = 0; i < r; i++) {
            threads[i] = new std::thread(play_with_nexts, p, c + i, scores);
        }

        for (int j = 0; j < r; j++) {
            threads[j]->join();
        }

        if (c > 0)
            std::cout << "\x1b[A"
                      << "\33[2K\r" << std::flush;

        std::cout << c << " / " << p->size << std::endl;

        c += r;
    }

    for (int i = 0; i < p->size; i++) {
        int v = scores[i];
        Chromosome *c = p->pop[i];

        if (v > b1) {
            b2 = b1, c2 = c1;
            b1 = v, c1 = c;
        } else if (v > b2 && v != b1) {
            b2 = v, c2 = c;
        }
    }

    play_match(c1, c2, 1);
    input.close();

    for (int i = 0; i < n; i++)
        delete threads[i];

    free(scores);
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
