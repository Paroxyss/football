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
#include "Genealogy.hpp"
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

gameStatistics Population::next(int n_thread, bool save, Generation *parent) {
    for (int i = 0; i < this->size; i++) {
        this->pop[i]->stats.instanceAge++;
    }

    SafeQueue<Chromosome *> nextPop;
    SafeQueue<gameStatistics> statsTournois;
    SafeQueue<std::tuple<int, int>> attributions;
    SafeQueue<carteIdentite> liens;

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
                                  &attributions, &liens]() {
            while (nextPop.reserve() < expected) {
                int tourn_size = thrand(4, this->size / 4);
                auto outcome = this->tournament(tourn_size, false);

                Chromosome *mutedWinner;
                if (likelyness(CROSSOVER_PROBABILITY)) {
                    auto c1 = std::get<0>(outcome);
                    auto c2 = std::get<1>(outcome);
                    mutedWinner = crossover(*c1, *c2);
                    liens.push((carteIdentite){
                        .id = mutedWinner->id, .p1 = c1->id, .p2 = c2->id});
                } else {
                    Chromosome *c = likelyness(0.8) ? std::get<0>(outcome)
                                                    : std::get<1>(outcome);
                    mutedWinner = cloneChromosome(c);
                    liens.push((carteIdentite){
                        .id = mutedWinner->id, .p1 = c->id, .p2 = 0});
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
        liens.push({.id = c->id, .p1 = 0, .p2 = 0});
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

    if (parent) {
        parent->arbre.ajouteCouche();
        while (liens.size()) {
            carteIdentite c;
            liens.pop(c);
            parent->arbre.pushId(c.id, c.p1, c.p2);
        }
    }

    return tourn_stats;
}

std::tuple<Chromosome *, Chromosome *, gameStatistics>
Population::tournament(int tourn_size, bool save) {
    std::queue<Chromosome *> contestants;

    bool *selected = (bool *)calloc(this->size, sizeof(bool));

    for (int i = 0; i < tourn_size; i++) {
        // TODO: Apparemment rand() n'est pas thread-safe.
        int k = thrand(0, this->size - 1);

        while (selected[k]) {
            k++;

            if (k == this->size) {
                k = 0;
            }
        }

        contestants.push(this->pop[k]);
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

    while (contestants.size() > 2) {
        auto c1 = contestants.front();
        contestants.pop();
        auto c2 = contestants.front();
        contestants.pop();
        auto match_results = play_match(c1, c2, save);

        if (match_results.score > 0) {
            contestants.push(c1);
        } else {
            contestants.push(c2);
        }

        // statistiques
        gameStats.totalCollisions += match_results.collisions;
        gameStats.totalGoals += match_results.goals;
        gameStats.total_ball_collisions += match_results.ball_collisions;
        gameStats.stopped += match_results.stopped ? 1 : 0;
        gameStats.n++;
    }

	auto w1 = contestants.front();
	auto w2 = contestants.back();

	auto final_result = play_match(w1, w2);

	Chromosome* winner;
	Chromosome* second;

	if (final_result.score > 0) {
		winner = w1;
		second = w2;
	} else {
		winner = w2;
		second = w1;
	}
	
	gameStats.totalCollisions += final_result.collisions;
	gameStats.totalGoals += final_result.goals;
	gameStats.total_ball_collisions += final_result.ball_collisions;
	gameStats.stopped += final_result.stopped ? 1 : 0;
	gameStats.n++;
	
    return std::make_tuple(winner, second, gameStats);
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
