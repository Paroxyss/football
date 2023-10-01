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
#include "SafeQueue.hpp"
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

void update_statistics(gameStatistics &tourn_stats,
                       gameStatistics &tournResult) {

    tourn_stats.totalCollisions += tournResult.totalCollisions;
    tourn_stats.totalGoals += tournResult.totalGoals;
    tourn_stats.n += tournResult.n;
    tourn_stats.total_ball_collisions += tournResult.total_ball_collisions;
    tourn_stats.stopped += tournResult.stopped;
}

gameStatistics Population::next(int n_thread, bool save) {
    for (int i = 0; i < this->size; i++) {
        this->pop[i]->stats.instanceAge++;
    }

    SafeQueue<Chromosome *> nextPop;
    SafeQueue<gameStatistics> statsTournois;
    SafeQueue<std::tuple<int, int>> attributions;

    int expected = (double)this->size * ((double)1 - (double)NEW_BLOOD);

    std::thread threads[n_thread];

    gameStatistics tourn_stats = {.n = 0,
                                  .totalCollisions = 0,
                                  .totalGoals = 0,
                                  .total_ball_collisions = 0,
                                  .stopped = 0};

    // on fait les tournois
    for (int i = 0; i < n_thread; i++) {
        threads[i] = std::thread([this, &nextPop, &expected, &statsTournois, i,
                                  &attributions]() {
            while (nextPop.reserve() < expected) {
                int tourn_size = random_power(this->size / 2);
                auto outcome = this->tournament(tourn_size, false);

                Chromosome *mutedWinner;
                if (likelyness(CROSSOVER_PROBABILITY)) {
                    mutedWinner =
                        crossover(*std::get<0>(outcome), *std::get<1>(outcome));
                } else {
                    Chromosome *c = likelyness(0.8) ? std::get<0>(outcome)
                                                    : std::get<1>(outcome);
                    mutedWinner = cloneChromosome(c);
                }

                mutate(*mutedWinner);
                nextPop.pushReserved(mutedWinner);
                statsTournois.push(std::get<2>(outcome));
                attributions.push(std::tuple(i, std::get<2>(outcome).n));
            }
        });
    }
    // on attends la fin des tournois
    for (int i = 0; i < n_thread; i++) {
        threads[i].join();
    }
    nextPop.clearReservations();

    // update_statistics(tourn_stats, &tournResult, matchs_count[i]);

    // On introduit des individus complètement nouveau pour explorer le plus
    // de solutions possible.
    while (nextPop.size() < this->size) {
        Chromosome *c = new Chromosome();
        c->initialize();
        nextPop.push(c);
    }

    if (nextPop.size() != this->size) {
        throw std::logic_error("Incohérence entre la population actuelle et la "
                               "population suivante");
    }

    for (int i = 0; i < this->size; i++) {
        Chromosome *c;
        nextPop.pop(c);

        delete this->pop[i];
        this->pop[i] = c;
    }

    std::vector<int> perfs(n_thread, 0);
    std::tuple<int, int> tstats;
    while (attributions.pop(tstats)) {
        perfs[std::get<0>(tstats)] += std::get<1>(tstats);
    }
    for (int i : perfs) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    while (statsTournois.size()) {
        gameStatistics s;
        statsTournois.pop(s);
        update_statistics(tourn_stats, s);
    }

    return tourn_stats;
}

std::tuple<Chromosome *, Chromosome *, gameStatistics>
Population::tournament(int tourn_size, bool save) {
    std::vector<Chromosome *> contestants(tourn_size);

    bool *selected = (bool *)calloc(tourn_size, sizeof(bool));

    for (int i = 0; i < tourn_size; i++) {
        // TODO: Apparemment rand() n'est pas thread-safe.
        int k = thrand(0, tourn_size - 1);

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
        .n = 0,
        .totalCollisions = 0,
        .totalGoals = 0,
        .total_ball_collisions = 0,
        .stopped = 0,
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
            gameStats.total_ball_collisions += match_results.ball_collisions;
            gameStats.stopped += match_results.stopped ? 1 : 0;
            gameStats.n++;
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
    clone->stats.instanceGoals = original->stats.instanceGoals;
    clone->stats.instanceAge = original->stats.instanceAge;

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
