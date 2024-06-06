#include "Population.hpp"
#include "util.hpp"
#include "Rand.hpp"

void simulate_and_save(const char *filename) {
	Generation g(0);
	g.load(filename);

	auto tourn_size = previous_power(g.currentPop->size);

	auto tourn = g.currentPop->tournament(tourn_size, 0, 2);
}

void play_random_match(const char *filename) {
	Generation g(0);
	g.load(filename);

	Chromosome *c1 = clone_chromosome(
            g.currentPop->pop[thrand(0, g.currentPop->size - 1)]);
	Chromosome *c2 = clone_chromosome(
            g.currentPop->pop[thrand(0, g.currentPop->size - 1)]);

	auto info = play_match(c1, c2, 1);
	while (info.ballCollisions == 0) {
		info = play_match(c1, c2, 1);
		std::cout << info << std::endl;
	}
}

void see_goal(const char *filename, int nGoal) {
	Generation g(0);
	g.load(filename);
	auto &p = g.currentPop;

    gameInformations info;
    do {
		Chromosome *c1 = clone_chromosome(p->pop[thrand(0, p->size - 1)]);
		Chromosome *c2 = clone_chromosome(p->pop[thrand(0, p->size - 1)]);

		info = play_match(c1, c2, 1);

		delete c1;
		delete c2;
	} while(info.goals >= nGoal);
}

void see_ball(const char *filename, int n_ball) {
	Generation g(0);
	g.load(filename);
	auto &p = g.currentPop;

    gameInformations info;
	do {
		Chromosome *c1 = clone_chromosome(p->pop[thrand(0, p->size - 1)]);
		Chromosome *c2 = clone_chromosome(p->pop[thrand(0, p->size - 1)]);

		info = play_match(c1, c2, 1);

		delete c1;
		delete c2;
	} while(info.ballCollisions >= n_ball);
}
