#include "Population.hpp"

#include <fstream>

/**
 * @brief On recherche les deux meilleures équipes pour afficher leur match.
 *
 * @param filename Fichier dans lequel récupérer la population.
 *
 */
void simulate_and_save(const char *filename) {
    Generation g(0);
    g.load(filename);

    auto tourn_size = previous_power(g.currentPop->size);

    auto tourn = g.currentPop->tournament(tourn_size, 0, 2);

    play_match(std::get<0>(tourn).front(), std::get<0>(tourn).back(), 1);
}

/**
 * @brief Simuler les deux meilleurs joueurs n'est pas toujours le plus cohérent
 * car ils peuvent être très similaires. La fonction permet de simuler un match
 * entre deux individus sélectionnés aléatoirement.
 */
void play_random_match(const char *filename) {
    Generation g(0);
    g.load(filename);

    Chromosome *c1 =
        cloneChromosome(g.currentPop->pop[thrand(0, g.currentPop->size - 1)]);
    Chromosome *c2 =
        cloneChromosome(g.currentPop->pop[thrand(0, g.currentPop->size - 1)]);

    play_match(c1, c2, 1);
}

void see_goal(const char *filename, int nGoal) {
    Generation g(0);
    g.load(filename);
	auto &p = g.currentPop;

    for (;;) {
        Chromosome *c1 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);
        Chromosome *c2 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);

        gameInformations info = play_match(c1, c2, 1);

		delete c1;
		delete c2;
        if (info.goals >= nGoal)
            break;
    }
}

void see_ball(const char *filename, int n_ball) {
    Generation g(0);
    g.load(filename);
	auto &p = g.currentPop;

    for (;;) {
        Chromosome *c1 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);
        Chromosome *c2 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);

        gameInformations info = play_match(c1, c2, 1);

		delete c1;
		delete c2;
        if (info.ball_collisions >= n_ball)
            break;
    }
}
