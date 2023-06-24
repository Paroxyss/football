#include "config.h"
#include "train.hpp"

#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        std::cerr << "Aucun argument fourni !" << std::endl
                  << "\t train <genN> <popSize> [nThread] : entraine le "
                     "réseaux en sauvegardant les populations"
                  << std::endl
                  << "\t play <popFile> : fait un tournoi au sein d'une "
                     "population et sauvegarde un mach entre les meilleurs"
                  << std::endl;
        _exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "train") == 0) {
        unsigned int genN = N;
        unsigned int popSize = POPULATION_SIZE;
        unsigned int nThread = std::thread::hardware_concurrency();
        if (argc >= 3) {
            genN = std::stoi(argv[2]);
        }
        if (argc >= 4) {
            popSize = std::stoi(argv[3]);
        }
        if (argc >= 5) {
            nThread = std::stoi(argv[4]);
        }
        train(genN, popSize, nThread);
    } else if (strcmp(argv[1], "play") == 0) {
        if (argc < 3) {
            throw std::invalid_argument("Missing argument popFile");
        }
        simulateAndSave(argv[2]);
    }

    _exit(EXIT_SUCCESS);
}
