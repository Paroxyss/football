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

int main() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    srand(0);
    Population p = Population(20);

    std::cout << "Matrix : " << sizeof(Matrix) << std::endl;
    std::cout << "Chromosome : " << sizeof(Chromosome) << std::endl;
    std::cout << "Population : " << sizeof(Population) << std::endl;

    for(int i = 0; i < 20; i++) {
		std::cout << std::endl << "===== GÉNÉRATION " << i  << " =====" << std::endl;
        p.next();
    }

    auto meilleurs = p.tournament(p.size, false);
    play_match(meilleurs.first, meilleurs.second, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    _exit(EXIT_SUCCESS);
}
