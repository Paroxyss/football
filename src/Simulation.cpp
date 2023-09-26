#include "Population.hpp"

#include <fstream>

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

/**
 * @brief Simuler les deux meilleurs joueurs n'est pas toujours le plus cohérent
 * car ils peuvent être très similaires. La fonction permet de simuler un match
 * entre deux individus sélectionnés aléatoirement.
 */
void play_random_match(const char *filename) {
    std::ifstream input;
    input.open(filename);

    if (!input.is_open())
        throw std::invalid_argument("File not found");

    Population *p = Population::read(input);

    Chromosome *c1 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);
    Chromosome *c2 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);

    play_match(c1, c2, 1);
    input.close();

    delete p;
}

void see_goal(const char *filename, int nGoal) {
    std::ifstream input;
    input.open(filename);

    if (!input.is_open())
        throw std::invalid_argument("File not found");

    Population *p = Population::read(input);

    for (;;) {
        Chromosome *c1 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);
        Chromosome *c2 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);

        gameInformations info = play_match(c1, c2, 1);

        if (info.goals >= nGoal)
            break;
    }

    input.close();
    delete p;
}

void see_ball(const char *filename, int n_ball) {
    std::ifstream input;
    input.open(filename);

    if (!input.is_open()) {
        throw std::invalid_argument("File not found");
    }

    Population *p = Population::read(input);

    for (;;) {
        Chromosome *c1 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);
        Chromosome *c2 = cloneChromosome(p->pop[thrand(0, p->size - 1)]);

        gameInformations info = play_match(c1, c2, 1);

        if (info.ball_collisions >= n_ball)
            break;
    }

    input.close();
    delete p;
}
