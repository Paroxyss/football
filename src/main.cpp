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
    srand(0);
    Population *p = new Population(10);
    int i = 0;

    p->next();

    delete p;

	_exit(EXIT_SUCCESS);
}
