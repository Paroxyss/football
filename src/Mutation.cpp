#include "Chromosome.hpp"
#include "Mutation.hpp"
#include "Rand.hpp"
#include "config.hpp"

double replacement() {
    return random_double();
}

double perturbation(double x) {
    return x + random_double(-x / 10., x / 10.);
}

double balance(double x) {
    if (likelyness(0.5)) {
        return perturbation(x);
    }

    return replacement();
}

void mutation(Matrix &m) {
    for (int i = 0; i < m.ligne; i++) {
        for (int j = 0; j < m.col; j++) {
            if (likelyness(1 - MUTATION_PROBABILITY)) {
                continue;
            }

            m.set(i, j, replacement());
        }
    }
}

void mutate(Chromosome &c) {
    // on mute le chromosome donc il perd en capacité, donc ses buts
    // précédents doivent être moins prépondérants
    c.stats.instanceGoals = (double)c.stats.instanceGoals / 2.;
    for (int i = 0; i < TEAM_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            mutation(*c.matrix[i][j]);
        }
    }

    if (likelyness(SWAP_MUTATION_PROBA)) {
        int a = thrand(0, TEAM_SIZE - 1);
        int b = thrand(0, TEAM_SIZE - 2);
        if (a == b)
            b++;

        for (int i = 0; i < NETWORK_SIZE - 1; i++) {
            for (int j = 0; j < c.matrix[a][i]->col; j++) {
                for (int k = 0; k < c.matrix[a][i]->ligne; k++) {
                    int tmp = c.matrix[a][i]->get(k, j);
                    c.matrix[a][i]->set(k, j, c.matrix[b][i]->get(k, j));
                    c.matrix[b][i]->set(k, j, tmp);
                }
            }
        }
    }
    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        mutation(*c.didier[i]);
    }
}
