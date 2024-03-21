#include "Chromosome.hpp"
#include "Game.hpp"
#include "Generation.hpp"
#include "Inputs.hpp"
#include "Simulation.hpp"
#include "config.h"
#include "train.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>

/*

    TODO: on prend à chaque génération un joueur aléatoire et on voit si sa
   valeur décroit. Si c'est le cas alors les neurones finissent par mourir il y
   a donc un problème.
*/

int main(int argc, char *argv[]) {

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
            << std::endl
            << "\t random <popFile> : sauvegarde un match entre deux individus "
               "choisis aléatoirement"
            << std::endl;
        _exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "train") == 0) {
        int n_gen = N;
        int pop_size = POPULATION_SIZE;
        int n_thread = std::thread::hardware_concurrency();

        if (argc >= 3) {
            n_gen = std::stoi(argv[2]);
        }
        if (argc >= 4) {
            pop_size = std::stoi(argv[3]);
        }
        if (argc >= 5) {
            n_thread = std::stoi(argv[4]);
        }
        char *fn = NULL;
        if (argc >= 6) {
            fn = argv[5];
        }

        // Il doit au moins être possible d'organiser un tournois
        if (pop_size < 4)
            throw std::invalid_argument("Population trop petite.");

        if (fn)
            trainFromFile(fn, n_gen, pop_size, n_thread);
        else
            train(n_gen, pop_size, n_thread);

    } else if (strcmp(argv[1], "genStats") == 0) {
        if (argc < 3) {
            throw std::invalid_argument("Missing argument popFile");
        }
        Generation g(0);
        g.load(argv[2]);
        g.saveJson("stats.json");
    } else if (strcmp(argv[1], "random") == 0) {
        play_random_match(argv[2]);
    } else if (strcmp(argv[1], "seegoal") == 0) {
        int nGoal = 1;
        if (argc > 3) {
            nGoal = std::stoi(argv[3]);
        }
        see_goal(argv[2], nGoal);
    } else if (strcmp(argv[1], "seeball") == 0) {
        int n_ball = 1;

        if (argc > 3) {
            n_ball = std::stoi(argv[3]);
        }

        see_ball(argv[2], n_ball);
    } else if (strcmp(argv[1], "rm") == 0) {
        Chromosome c1;
        Chromosome c2;
        c1.initialize();
        c2.initialize();

        play_match(&c1, &c2, true);
    } else if (strcmp(argv[1], "testSave") == 0) {
        Chromosome c1;
        c1.initialize();
        std::ofstream file;
        file.open("testpop.pop");
        c1.write(file);
        file.close();

        std::ifstream file2;
        file2.open("testpop.pop");
        auto c2 = Chromosome::read(file2);
        file2.close();

        std::cout << "#############" << std::endl;
        c1.print();
        std::cout << "#############" << std::endl;
        c2->print();
        std::cout << "#############" << std::endl;
        delete c2;

    } else if (strcmp(argv[1], "rm2") == 0) {
        std::cout << "simulation random" << std::endl;
        auto g = Game(2 * EQUIPE_SIZE, true);
        g.ball.pos = {.x = MAP_LENGTH / 2., .y = MAP_HEIGHT / 2.};
        g.ball.vitesse = {.x = 0, .y = 0};

        int c[] = GAMECONFIG;
        g.set_players(c, GAMECONFIGLENGTH);

        int to_touch = MAX_TOUCH_DURATION;

        for (int k = 0; k < 300; k++, to_touch--) {
            writeInputs(g.players[0], g.players, g.players + 3, &g.ball, true);
            g.players->orientation += 1 / 10. * (k < 150);
            g.players->vitesse.x = cos(k / 10.) * 5;
            g.players->vitesse.y = sin(k / 10.) * 5;
            // on tick 10 fois pour beaucoup plus de précisions
            g.tick(0.1, true, false, true);
            for (int i = 0; i < 8; i++) {
                g.tick(0.1, true, false, false);
            }
            g.tick(0.1, true, true, false);
        }

    } else if (strcmp(argv[1], "benchmarkMatch") == 0) {
        int matchPerIter = 100;
        int chromosomeCount = 20;

        unsigned long nbMatch = 0;
        double totalTime = 0;
        Chromosome c[chromosomeCount];
        std::vector<double> valeurs;
        std::vector<double> vc;
        while (true) {
            for (int i = 0; i < chromosomeCount; i++) {
                c[i].initialize();
            }
            auto start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < matchPerIter; i++) {
                play_match(&c[i % chromosomeCount], &c[i / chromosomeCount]);
            }

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;

            nbMatch += matchPerIter;

            valeurs.push_back(matchPerIter / elapsed_seconds.count());

            double mean =
                std::reduce(valeurs.begin(), valeurs.end()) / valeurs.size();
            vc = valeurs;
            std::transform(valeurs.begin(), valeurs.end(), vc.begin(),
                           [mean](double v) { return pow(mean - v, 2); });

            double incertitude =
                sqrt(std::reduce(vc.begin(), vc.end()) / (vc.size() - 1));

            totalTime += elapsed_seconds.count();
            std::cout << mean << " ± " << incertitude << " matchs par seconde "
                      << "(" << nbMatch << " / " << totalTime << "s)"
                      << std::endl;
        }
    }

    _exit(EXIT_SUCCESS);
}
