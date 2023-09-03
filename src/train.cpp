#include <filesystem>
#include <stdexcept>
namespace fs = std::__fs::filesystem;

#include "Chromosome.hpp"
#include "Population.hpp"
#include "config.h"
#include "util.hpp"

#define POPNAME(gen)                                                           \
    "pops/pop-gen" + std::to_string(gen) + "-at-" + std::to_string(time(NULL))

void train(int n_gen, int population_size, int n_thread) {

    std::cout << "Starting a train of " << n_gen << " generations with "
              << population_size << " chromosomes on " << n_thread
              << " threads." << std::endl;

    int gen = 0;
    int last_save = 0;

    Population *pop = new Population(population_size);
    pop->initialize();

    while (gen < n_gen) {

        auto start = std::chrono::steady_clock::now();

        gameStatistics stats = pop->next(n_thread);

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        gen += 1;
        last_save += 1;

        std::cout << "\nStats gen " << gen << " " << stats << " in "
                  << elapsed_seconds.count() << std::endl;

        if (last_save > SAVE_RATE) {
            std::ofstream out;
            auto backup_fname = POPNAME(gen);
            out.open(backup_fname);

            pop->write(out);
            out.close();

            fs::remove("pops/latest-backup");
            fs::create_hard_link(backup_fname, "pops/latest-backup");
            last_save = 0;
        }
    }

    std::ofstream out;
    auto outFName = POPNAME(gen);
    out.open(outFName);

    pop->write(out);
    out.close();
    fs::remove("pops/latest");
    fs::create_hard_link(outFName, "pops/latest");

    delete pop;
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
