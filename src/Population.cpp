#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>
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
#include "ProgressBar.hpp"

Population::Population(int size, double proportionDidier) {
    this->size = size;
    this->pop = new Chromosome *[size];

    for (int i = 0; i < size; i++) {
        this->pop[i] = new Chromosome();
		this->pop[i]->hasDidier = likelyness(proportionDidier);
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

    SafeQueue<Chromosome *> nextPop(n_thread);
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

	ProgressBar pbar(this->size);
    // on fait les tournois
    for (int threadId = 0; threadId < n_thread; threadId++) {
        threads[threadId] = std::thread([this, &nextPop, &expected,
                                         &statsTournois, threadId,
                                         &attributions, &liens, &pbar]() {
            while (nextPop.reserve(threadId, NB_PAR_TOURNOI) < expected) {
                int tourn_size = thrand(4+NB_PAR_TOURNOI, this->size * PRESSION_SELECTIVE);
                auto outcome =
                    this->tournament(tourn_size, NB_PAR_TOURNOI, false);

                auto q = std::get<0>(outcome);
                std::vector<Chromosome *> vainqueurs;
                while (!q.empty()) {
                    vainqueurs.push_back(q.front());
                    q.pop();
                }
                for (int i = 0; i < vainqueurs.size(); i++) {
                    Chromosome *mutedWinner;
					auto c1 = vainqueurs[i];
					auto c2 = vainqueurs[(i+1)%vainqueurs.size()];
                    if (likelyness(CROSSOVER_PROBABILITY)) {
                        mutedWinner = crossover(*c1, *c2);
                        liens.push((carteIdentite){
                            .id = mutedWinner->id, .p1 = c1->id, .p2 = c2->id});
                    } else {
                        mutedWinner = cloneChromosome(c1);
                        liens.push((carteIdentite){
                            .id = mutedWinner->id, .p1 = c1->id, .p2 = 0});
                    }

                    mutate(*mutedWinner);
					pbar.step(1, threadId);
                    nextPop.pushReserved(mutedWinner, threadId);
                    statsTournois.push(std::get<1>(outcome));
                    attributions.push(
                        std::tuple(threadId, std::get<1>(outcome).n));
                }
            }
            nextPop.cancelRes(threadId);

            // On introduit des individus complètement nouveau pour explorer le
            // plus de solutions possible.
            while (nextPop.reserve(threadId, 1) <= this->size) {
                Chromosome *c = new Chromosome();
                c->initialize();
				pbar.step(1, threadId);
                nextPop.pushReserved(c, threadId);
                liens.push({.id = c->id, .p1 = 0, .p2 = 0});
            }
        });
    }
    // on attends la fin des tournois
    for (int i = 0; i < n_thread; i++) {
        threads[i].join();
    }
    nextPop.clearReservations();

    if (nextPop.size() != this->size) {
        std::cout << nextPop.size() << "≠" << this->size << std::endl;
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

std::tuple<std::queue<Chromosome *>, gameStatistics>
Population::tournament(int tourn_size, int maxSize, bool save) {
	if(tourn_size < maxSize){
		std::cout << tourn_size << " " << maxSize << std::endl;
		throw std::logic_error("tourn_size < maxSize");
	};
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

    while (contestants.size() > maxSize) {
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

    return std::make_tuple(contestants, gameStats);
}

Chromosome *cloneChromosome(Chromosome *original) {
    auto clone = new Chromosome();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
			Matrix::clone(original->matrix[i][j], clone->matrix[i][j]);
        }
    }

	for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++){
		Matrix::clone(original->didier[i], clone->didier[i]);
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
