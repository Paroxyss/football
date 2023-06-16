#include "Chromosome.hpp"
#include "Game.hpp"
#include "Matrix.h"
#include "Population.hpp"
#include "config.h"
#include "util.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <thread>
#include <unistd.h>

int main() {
	auto c1 = Chromosome();
	c1.randomize();
	auto c2 = Chromosome();
	c2.randomize();

	std::cout << "starting game" << std::endl;
	Game::play_match(&c1, &c2);
}
