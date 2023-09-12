#include "Population.hpp"

void trainPop(Population* pop, int n_gen, int population_size, int n_thread);
void trainFromFile(const char* inputFile, int n_gen, int population_size, int n_thread);
void train(int n_gen, int pop_size, int n_thread);
void saveMap(const char *filename, const char *outputFn = "map.csv");
