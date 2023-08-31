#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "Chromosome.hpp"
#include "Game.hpp"
#include "Population.hpp"
#include "config.h"
#include "util.hpp"

Population::Population(int size) {
    this->size = size;
    this->pop = new Chromosome *[size];

    for (int i = 0; i < size; i++) {
        this->pop[i] = new Chromosome();
    }
}

Population::~Population() {
    for (int i = 0; i < size; i++) {
        delete this->pop[i];
    }

    delete[] this->pop;
}

void Population::initialize() {
    for (int i = 0; i < this->size; i++) {
        this->pop[i]->initialize();
    }
}

void update_statistics(gameStatistics &tourn_stats, gameStatistics *tournResult,
                       int matchs_count) {

    tourn_stats.totalCollisions += tournResult->totalCollisions;
    tourn_stats.totalGoals += tournResult->totalGoals;
    tourn_stats.n += matchs_count;
}

gameStatistics Population::next(int n_thread, bool save) {
    Chromosome **nxt = new Chromosome *[this->size];

    int count = 0;
    int expected = this->size * (1 - NEW_BLOOD);

    std::thread threads[n_thread];
    std::tuple<Chromosome *, Chromosome *, gameStatistics> winners[n_thread];

    gameStatistics tourn_stats = {
        .n = 0,
        .totalCollisions = 0,
        .totalGoals = 0,
    };

    // Pour la barre de progression, ne marche pas très bien pour les petites
    // populations mais bon
    int outRate = std::floor(expected / 10);
    int *matchs_count = new int[n_thread];

    while (count < expected) {
        if (likelyness(CROSSOVER_PROBABILITY)) {

            if (expected - count < n_thread) {
                n_thread = 1;
            }

            for (int i = 0; i < n_thread; i++) {
                // on choisit une puissance de 2 aléatoire car ça permet
                // d'organiser des petites compétitions et donc limiter la
                // pression selective.
                // On divise par 4 sinon c'est beaucoup trop lent.
                int tourn_size = random_power(this->size / 2);

                matchs_count[i] = tourn_size - 1;

                threads[i] =
                    std::thread([this, tourn_size, save, &winners, i]() {
                        auto outcome = this->tournament(tourn_size, save);
                        winners[i] = outcome;
                    });
            }

            for (int i = 0; i < n_thread; ++i) {
                threads[i].join();
            }

            for (int i = 0; i < n_thread; i++) {
                nxt[count] = crossover(*std::get<0>(winners[i]),
                                       *std::get<1>(winners[i]));

                auto tournResult = std::get<2>(winners[i]);
                update_statistics(tourn_stats, &tournResult, matchs_count[i]);

                if ((count % outRate) == 0) {
                    std::cout << "*";
                    fflush(stdout);
                }

                count++;
            }
        } else {
            nxt[count] = cloneChromosome(this->pop[rand() % this->size]);
            count++;
        }
    }

    // On introduit des individus complètement nouveau pour explorer le plus de
    // solutions possible.
    while (count < this->size) {
        nxt[count] = new Chromosome();
        nxt[count]->initialize();

        count++;
    }

    for (int i = 0; i < this->size; i++) {
        mutate(*nxt[i]);

        delete this->pop[i];
        this->pop[i] = nxt[i];
    }

    delete[] nxt;
    delete[] matchs_count;
    return tourn_stats;
}

std::tuple<Chromosome *, Chromosome *, gameStatistics>
Population::tournament(int tourn_size, bool save) {
    std::vector<Chromosome *> contestants(tourn_size);

    bool *selected = (bool *)calloc(tourn_size, sizeof(bool));

    for (int i = 0; i < tourn_size; i++) {
        // TODO: Apparemment rand() n'est pas thread-safe.
        int k = rand() % tourn_size;

        while (selected[k]) {
            k++;

            if (k == tourn_size) {
                k = 0;
            }
        }

        contestants[i] = this->pop[k];
        selected[k] = true;
    }

    free(selected);

    gameStatistics gameStats = {
        .totalCollisions = 0,
        .totalGoals = 0,
    };

    while (tourn_size > 2) {
        int pool_size = tourn_size / 2;
        std::vector<Chromosome *> pool(pool_size);

        for (int i = 0; i < pool_size; i++) {
            auto match_results =
                play_match(contestants[i * 2], contestants[i * 2 + 1], save);

            if (match_results.score > 0) {
                pool[i] = contestants[i * 2];
            } else {
                pool[i] = contestants[i * 2 + 1];
            }

            // statistiques
            gameStats.totalCollisions += match_results.collisions;
            gameStats.totalGoals += match_results.goals;
        }

        tourn_size = pool_size;
        contestants = pool;
    }

    return std::make_tuple(contestants[0], contestants[1], gameStats);
}

Chromosome *cloneChromosome(Chromosome *original) {
    auto clone = new Chromosome();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {

            for (int k = 0; k < original->matrix[i][j]->ligne; k++) {
                for (int l = 0; l < original->matrix[i][j]->col; l++) {
                    clone->matrix[i][j]->set(k, l,
                                             original->matrix[i][j]->get(k, l));
                }
            }
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        for (int k = 0; k < original->didier[i]->ligne; k++) {
            for (int l = 0; l < original->didier[i]->col; l++) {
                clone->didier[i]->set(k, l, original->didier[i]->get(k, l));
            }
        }
    }

    return clone;
}

void Population::write(std::ofstream &file) {
    file.write((char *)&this->size, sizeof(this->size));

    for (int i = 0; i < this->size; i++) {
        this->pop[i]->write(file);
    }
}

Population *Population::read(std::ifstream &file) {
    int size;
    file.read((char *)&size, sizeof(size));

    Population *c = new Population(size);

    for (int i = 0; i < c->size; i++) {
        delete c->pop[i];
        c->pop[i] = Chromosome::read(file);
    }

    return c;
};
