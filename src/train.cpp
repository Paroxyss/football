#include <filesystem>
#include <stdexcept>
namespace fs = std::__fs::filesystem;

#include "Chromosome.hpp"
#include "Generation.hpp"
#include "Population.hpp"
#include "config.h"
#include "train.hpp"
#include "util.hpp"

#define POPNAME(gen)                                                           \
    "pops/pop-gen" + std::to_string(gen) + "-at-" + std::to_string(time(NULL))

void train(int n_gen, int population_size, int n_thread) {
    Generation g((unsigned int)n_thread);
    g.createPopulation(population_size);
    std::cout << "Nouvelle génération crée" << std::endl;
    trainPop(g, n_gen, n_thread);
}

void trainFromFile(const char *inputFile, int n_gen, int population_size,
                   int n_thread) {
    Generation g(n_thread);
    g.load(inputFile);
    std::cout << "Génération " << g.generation << " chargée ("
              << g.currentPop->size << " individus)" << std::endl;
    trainPop(g, n_gen, n_thread);
}

void trainPop(Generation &g, int n_gen,  int n_thread) {
    std::cout << "Starting a train of " << n_gen << " generations with "
              << g.currentPop->size << " chromosomes on " << n_thread
              << " threads." << std::endl;

    int last_save = 0;
    g.rewriteStats();

    unsigned int initialGeneration = g.generation;
    while (g.generation - initialGeneration < n_gen) {

        auto start = std::chrono::steady_clock::now();

        g.step();

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        last_save += 1;

        auto stats = g.stats.back();
        double n = (double)stats.n;

        std::cout << "Stats gen " << g.generation << " " << stats << " in "
                  << elapsed_seconds.count() << std::endl;

        if (g.generation % SAVE_RATE == 0 && g.generation > 0) {
            auto backup_fname = POPNAME(g.generation);
            g.save(backup_fname);

            fs::remove("pops/latest-backup");
            fs::create_hard_link(backup_fname, "pops/latest-backup");
        }
    }

    auto fn = POPNAME(g.generation);
    g.save(fn);
    fs::remove("pops/latest");
    fs::create_hard_link(fn, "pops/latest");
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
