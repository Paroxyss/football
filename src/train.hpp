#include "Population.hpp"

void generation(Population *pop, gameStatistics *res);
void train(unsigned int generationObjective, unsigned int populationSize,
           unsigned int nbThread);

void simulateAndSave(const char* filename);
