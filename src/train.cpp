#include <filesystem>
#include <stdexcept>
namespace fs = std::__fs::filesystem;

#include "Chromosome.hpp"
#include "Population.hpp"
#include "config.h"
#include "train.hpp"
#include "Generation.hpp"
#include "util.hpp"

#define POPNAME(gen)                                                           \
    "pops/pop-gen" + std::to_string(gen) + "-at-" + std::to_string(time(NULL))

void train(int n_gen, int population_size, int n_thread) {
	Generation g((unsigned int)n_thread);
	g.createPopulation(population_size);
    std::cout << "Nouvelle population crée" << std::endl;
    trainPop(g, n_gen, population_size, n_thread);
}

void trainFromFile(const char *inputFile, int n_gen, int population_size,
                   int n_thread) {
    /*    std::ifstream input;
        input.open(inputFile);
        Generation *g = Population::read(input);
        std::cout << "Population chargée depuis " << inputFile << std::endl;
        trainPop(pop, n_gen, population_size, n_thread);
        delete pop;*/
}

void trainPop(Generation &g, int n_gen, int population_size, int n_thread) {
    std::cout << "Starting a train of " << n_gen << " generations with "
              << population_size << " chromosomes on " << n_thread
              << " threads." << std::endl;

    int last_save = 0;

    while (g.generation < n_gen) {

        auto start = std::chrono::steady_clock::now();

        g.step();

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        last_save += 1;

        auto stats = g.stats.front();
        double n = (double)stats.n;

        std::cout << "Stats gen " << g.generation << " " << stats << " in "
                  << elapsed_seconds.count() << std::endl;

		g.saveArbre("arbreDeVie.json");
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

        input.close();

        delete p;
    }
    output.close();
}
