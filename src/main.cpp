#include "Chromosome.hpp"
#include "Game.hpp"
#include "Matrix.h"
#include "Network.hpp"
#include "config.h"
#include "util.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <thread>
#include <unistd.h>

void printCollisionList(collisionList *list) {
    if (list == NULL) {
        std::cout << "Fin de liste" << std::endl;
        return;
    }

    if (list->type == CIRCLE) {
        std::cout << "Collision avec une balle: x: " << list->obj->pos.x
                  << " y: " << list->obj->pos.y << std::endl;
    } else {
        std::cout << "Collision avec un mur " << list->obj->pos << " "
                  << list->obj->vitesse << std::endl;
    }
    printCollisionList(list->next);
}

int main() {
    auto g = Game(1);

    g.players[0].pos = {.x = 300, .y = 500};
    g.players[0].vitesse = {.x = 30, .y = -60};

    g.ball.pos = {.x = 600, .y = 150};
    g.ball.vitesse = {.x = 1, .y = 10};

    int i = 0;
    /*
        g.tick();
        g.print();
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        g.tick();
        g.print();
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        collisionList *list = g.getCollisionList(0);
        std::cout << list << std::endl;

        fflush(stdout);
        // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        auto &pl = g.players[0];
        std::cout << "Player : " << pl.pos << " " << pl.vitesse << " " <<
       pl.size
                  << " ball size " << g.ball.size << std::endl;
        auto res = backTrackCollisions(list, pl);

        g.print();

        printCollisionList(list);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        g.print();

        std::cout << "Player : pos : " << pl.pos << " " << pl.vitesse << " "
                  << pl.size << " ball size " << g.ball.size << std::endl;
        std::cout << "Got p: " << res.distanceBefore << std::endl;

        g.players[0].pos += pl.vitesse / norme(pl.vitesse) * res.distanceBefore;

        std::cout << "Player : pos : " << pl.pos << " " << pl.vitesse << " "
                  << pl.size << " ball size " << g.ball.size << std::endl;

        computeCollision(res.obj, pl);

        std::cout << "Player : pos : " << pl.pos << " " << pl.vitesse << " "
                  << pl.size << " ball size " << g.ball.size << std::endl;

        */
    while (true) {
		/*
        g.tick();
        collisionList *list = g.getCollisionList(0);
        if (list) {
            auto &pl = g.players[0];
            auto res = backTrackCollisions(list, pl);
            g.players[0].pos +=
                pl.vitesse / norme(pl.vitesse) * res.distanceBefore;
            computeCollision(res.obj, pl);
        }
		*/
		vector posa = g.players[0].pos;
		g.tickBall(0);
		vector posb = g.players[0].pos;
		g.tickBall(-1);
        g.print();
		std::cout << g.players[0].pos << "v :" << g.players[0].vitesse << std::endl;
		std::cout << posb-posa<< std::endl;
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    exit(0);

    while (true) {

        for (int i2 = 0; i2 < g.playerNumber; i2++) {
            if (i % (10 + i2) == 0) {
                g.players[i2].orientation += 0.5;
            }

            if (i % (13 + 4 * i2) == 0) {
                g.players[i2].acceleration = 20;
            }
        }

        g.tick();
        g.print();
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i++;
    }
    g.print();
    fflush(stdout);
}
