#include "Matrix.h"
#include "config.h"
#include "train.hpp"

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>

/*

    TODO: on prend à chaque génération un joueur aléatoire et on voit si sa
   valeur décroit. Si c'est le cas alors les neurones finissent par mourir il y
   a donc un problème.
*/

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        std::cerr
            << "Aucun argument fourni ! " << std::endl
            << "\t train <genN> <popSize> : entraine le "
               "réseaux en sauvegardant les populations"
            << std::endl
            << "\t play <popFile> : fait un tournoi au sein d'une "
               "population et sauvegarde un mach entre les meilleurs"
            << std::endl
            << "\t map <popFile> [output] crée un fichier csv mappant une "
               "population"
            << std::endl;
        _exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "train") == 0) {
        unsigned int n_gen = N;
        unsigned int pop_size = POPULATION_SIZE;

        if (argc >= 3) {
            n_gen = std::stoi(argv[2]);
        }
        if (argc >= 4) {
            pop_size = std::stoi(argv[3]);
        }

        // Il doit au moins être possible d'organiser un tournois
        if (pop_size < 4) {
            throw std::invalid_argument("Population trop petite.");
        }

        train(n_gen, pop_size);
    } else if (strcmp(argv[1], "play") == 0) {
        if (argc < 3) {
            throw std::invalid_argument("Missing argument popFile");
        }
        simulate_and_save(argv[2]);
    } else if (strcmp(argv[1], "map") == 0) {
        if (argc < 3) {
            throw std::invalid_argument("Missing argument popFile");
        }
        if (argc > 3) {
            saveMap(argv[2], argv[3]);
        } else {
            saveMap(argv[2]);
        }
    }

    _exit(EXIT_SUCCESS);
}
