#pragma once

#include "config.h"
#include "Matrix.h"
#include <array>

class Network {
  public:
    Network(std::array<unsigned int, NETWORK_SIZE> n);
	~Network();
    Matrix **weight;

    std::array<double, NETWORK_OUTPUT_SIZE> evaluate(std::array<double, NETWORK_INPUT_SIZE> input);

    // deviation is in proportion
    void mutate(double rate, double deviation);
    void randomize();

    void print();
};
