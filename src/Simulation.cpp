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

    Chromosome *c1 = cloneChromosome(p->pop[rand() % p->size]);
    Chromosome *c2 = cloneChromosome(p->pop[rand() % p->size]);

    play_match(c1, c2, 1);
    input.close();

    delete p;
}

void see_goal(const char *filename) {
    std::ifstream input;
    input.open(filename);

    if (!input.is_open())
        throw std::invalid_argument("File not found");

    Population *p = Population::read(input);

    for (;;) {
        Chromosome *c1 = cloneChromosome(p->pop[rand() % p->size]);
        Chromosome *c2 = cloneChromosome(p->pop[rand() % p->size]);

        gameInformations info = play_match(c1, c2, 1);

        if (info.goals != 0)
            break;
    }

    input.close();
    delete p;
}
