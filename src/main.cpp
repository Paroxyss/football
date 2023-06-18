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

void generation(Population *pop, int amount) {
    for (int i = 0; i < amount; i++) {
        pop->next();
    }
}

int main() {
    srand(0);

    Population pops[] = {Population(100), Population(100), Population(100), Population(100)};

    for (int i = 0; i < 1; i++) {
		std::cout << "Generation " << 2*i << std::endl;
		std::thread t1(generation, &pops[0], 2);
		std::thread t2(generation, &pops[1], 2);
		std::thread t3(generation, &pops[2], 2);
		std::thread t4(generation, &pops[3], 2);

		t1.join();
		t2.join();
		t3.join();
		t4.join();

		shufflePopulations(pops, 4);
    }

    _exit(EXIT_SUCCESS);
}
