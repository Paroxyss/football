#include "Population.hpp"

void train(unsigned int generationObjective, unsigned int populationSize,
           unsigned int nbThread);

void simulate_and_save(const char *filename);
void saveMap(const char *filename, const char *outputFn = "map.csv");
