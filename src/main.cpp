#include "Chromosome.hpp"
#include "Game.hpp"
#include "Matrix.h"
#include "Population.hpp"
#include "config.h"
#include "util.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <thread>
#include <unistd.h>

// std::cout << randomDouble() << std::endl;
// int i = 0;
// srand(time(NULL));
// while (1) {
//     long seed = time(NULL) % 1000000 + 1000000 * i;
//     long seedC = time(NULL) * 1000000 + i;

//     srand(seedC);
//     auto c1 = Chromosome();
//     c1.randomize();
//     auto c2 = Chromosome();
//     c2.randomize();

//     srand(seed);
//     int score = play_match(&c1, &c2, false);

//     std::cout << "Partie [" << seed << " | " << seedC << " ] -> " << score
//               << std::endl;
//     if (abs(score) >= 2) {
//         srand(seed);
//         score = play_match(&c1, &c2, true);
//         exit(0);
//     }
//     i++;
// }

int main() {

    Population *p = new Population(10);
    int i = 0;

    while (i++ < 100) {
        p->next(i == 100);
    }

    delete p;
}
