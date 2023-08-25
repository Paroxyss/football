#include <stdexcept>

#include "Chromosome.hpp"
#include "Population.hpp"
#include "config.h"
#include "util.hpp"

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
            out.open("pops/pop-gen" + std::to_string(gen) + "-at-" +
                     std::to_string(time(NULL)));

            pop->write(out);

            out.close();
            last_save -= SAVE_RATE;
        }
    }

    std::ofstream out;
    out.open("pops/pop-gen" + std::to_string(gen) + "-at-" +
             std::to_string(time(NULL)));

    pop->write(out);
    out.close();

    delete pop;
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
 */
void simulate_and_save(const char *filename) {
    std::ifstream input;
    input.open(filename);

    if (!input.is_open())
        throw std::invalid_argument("File not found");

    Population *pop = Population::read(input);

    auto tourn_size = previous_power(pop->size);

    auto tourn = pop->tournament(tourn_size, 0);

    play_match(std::get<0>(tourn), std::get<1>(tourn), 1);

    input.close();
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
