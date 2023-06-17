#include <cstdlib>
#include <iostream>
#include <malloc/_malloc.h>
#include <utility>

#include "Chromosome.hpp"
#include "Game.hpp"
#include "Population.hpp"

Population::Population(int size) {
    this->size = size;
    this->pop = (Chromosome *)malloc(size * sizeof(Chromosome));

    for (int i = 0; i < size; i++) {
        this->pop[i] = *new Chromosome();
        this->pop[i].randomize();
    }
}

Population::~Population() {
    free(this->pop);
}

/*
    De ce que je me souvent une méthode classique est de généré
    à chaque générations entre 80 et 90% des individus par reproduction,
    1% par mutation et le reste par clonage.
*/

void Population::next(bool save) {
    Population *p = new Population(this->size);
    int c = 0, m = 0;

    while (c < this->size * 0.85) {
        auto cpl = this->tournament(rand() % (this->size / 2) + 1, save);

        // il y a certaines méthodes de crossover qui retournent directement 2
        // enfants par exemple one_pointer_crossover(a, b) !=
        // one_pointer_crossover(b, a) on peut donc avoir les 2;
        p->pop[c++] = *crossover(cpl.first, cpl.second);
    }

    while (m < this->size * 0.1) {
        p->pop[c++] = *mutate(&this->pop[rand() % this->size]);
        m++;
    }

    while (c + m < this->size) {
        p->pop[c + m++] = this->pop[rand() % this->size];
    }

    for (int i = 0; i < this->size; i++) {
        this->pop[i] = p->pop[i];
    }

    delete p;
}

std::pair<Chromosome *, Chromosome *> Population::tournament(int k, bool save) {
    Chromosome *r = (Chromosome *)malloc(k * sizeof(Chromosome));
    std::pair<Chromosome *, Chromosome *> p;

    for (int i = 0; i < k; i++) {
        r[i] = this->pop[rand() % this->size];
    }

    for (int i = 1; i < k; ++i) {
        Chromosome c = this->pop[i];
        int j = i - 1;

        while (j >= 0 && play_match(&this->pop[j], &c, save) > 0) {
            this->pop[j + 1] = this->pop[j];
            --j;
        }

        this->pop[j + 1] = c;
    }

    p.first = &r[k - 1];
    p.second = &r[k - 2];

    return p;
}
