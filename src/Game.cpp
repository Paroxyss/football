#include "Game.hpp"
#include "Chromosome.hpp"
#include "Matrix.hpp"
#include "Rand.hpp"
#include "Vector.hpp"
#include "config.hpp"
#include "stdlib.h"
#include "util.hpp"

#include <cmath>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <math.h>
#include <ostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

std::ofstream csvOutputFile;
#define SETWALLBOUT(id, px, py)                                            \
    this->wallsBouts[id].pos.x = px;                                       \
    this->wallsBouts[id].pos.y = py;                                       \
    this->wallsBouts[id].vitesse.x = 0;                                    \
    this->wallsBouts[id].vitesse.y = 0;                                    \
    this->wallsBouts[id].mass = 10e100;                                    \
    this->wallsBouts[id].size = 0;

#define SETWALL(id, px, py, dx, dy)                                        \
    this->walls[id].pos.x = px;                                            \
    this->walls[id].pos.y = py;                                            \
    this->walls[id].vitesse.x = dx;                                        \
    this->walls[id].vitesse.y = dy;                                        \
    SETWALLBOUT(2 * id, px, py);                                           \
    SETWALLBOUT(2 * id + 1, px + dx, py + dy);

#define SETGOAL(id, px, py, dx, dy, s)                                     \
    this->goals[id].pos.x = px;                                            \
    this->goals[id].pos.y = py;                                            \
    this->goals[id].vitesse.x = dx;                                        \
    this->goals[id].vitesse.y = dy;                                        \
    this->goals[id].size = s;

Game::Game(int playerNumber, bool logToFile) {
    this->logToFile = logToFile;

    this->playerNumber = playerNumber;
    this->players = new player[playerNumber];

    this->wallNumber = 16;
    this->walls = new wall[this->wallNumber];
    // debuts et fins des murs
    this->wallsBouts = new struct ball[2 * this->wallNumber];

    this->goals = new wall[2];

    // Bordures du terrain
    SETWALL(0, 0, 0, MAP_LENGTH, 0);
    SETWALL(1, 0, 0, 0, MAP_HEIGHT);
    SETWALL(2, 0, MAP_HEIGHT, MAP_LENGTH, 0);
    SETWALL(3, MAP_LENGTH, MAP_HEIGHT, 0, -MAP_HEIGHT);

    // Cages de l'équipe de gauche
    SETWALL(4, 0, (MAP_HEIGHT - (double)GOAL_HEIGHT) / 2 - POLE_WIDTH,
            POLE_LENGTH, 0);
    SETWALL(5, 0, (MAP_HEIGHT - (double)GOAL_HEIGHT) / 2, POLE_LENGTH, 0);
    SETWALL(6, 0, (MAP_HEIGHT + (double)GOAL_HEIGHT) / 2 + POLE_WIDTH,
            POLE_LENGTH, 0);
    SETWALL(7, 0, (MAP_HEIGHT + (double)GOAL_HEIGHT) / 2, POLE_LENGTH, 0);
    SETWALL(8, POLE_LENGTH,
            (MAP_HEIGHT - (double)GOAL_HEIGHT) / 2 - POLE_WIDTH, 0,
            POLE_WIDTH);
    SETWALL(9, POLE_LENGTH, (MAP_HEIGHT + (double)GOAL_HEIGHT) / 2, 0,
            POLE_WIDTH);

    // Cages de l'équipe de droite
    SETWALL(10, MAP_LENGTH,
            (MAP_HEIGHT - (double)GOAL_HEIGHT) / 2 - POLE_WIDTH,
            -POLE_LENGTH, 0);
    SETWALL(11, MAP_LENGTH, (MAP_HEIGHT - (double)GOAL_HEIGHT) / 2,
            -POLE_LENGTH, 0);
    SETWALL(12, MAP_LENGTH,
            (MAP_HEIGHT + (double)GOAL_HEIGHT) / 2 + POLE_WIDTH,
            -POLE_LENGTH, 0);
    SETWALL(13, MAP_LENGTH, (MAP_HEIGHT + (double)GOAL_HEIGHT) / 2,
            -POLE_LENGTH, 0);
    SETWALL(14, MAP_LENGTH - POLE_LENGTH,
            (MAP_HEIGHT - (double)GOAL_HEIGHT) / 2 - POLE_WIDTH, 0,
            POLE_WIDTH);
    SETWALL(15, MAP_LENGTH - POLE_LENGTH,
            (MAP_HEIGHT + (double)GOAL_HEIGHT) / 2, 0, POLE_WIDTH);

    SETGOAL(0, 0,
            static_cast<float>(MAP_HEIGHT) / 2 -
                static_cast<float>(GOAL_HEIGHT) / 2,
            0, 1, GOAL_HEIGHT);
    SETGOAL(1, MAP_LENGTH,
            static_cast<float>(MAP_HEIGHT) / 2 -
                static_cast<float>(GOAL_HEIGHT) / 2,
            0, 1, GOAL_HEIGHT);

    set_ball({.x = 0, .y = 0}, {.x = 0, .y = 0}, BALL_SIZE);
    for (int i = 0; i < playerNumber; i++) {
        set_player(i, {.x = 0, .y = 0}, {.x = 0, .y = 0}, 0, PLAYER_SIZE);
        players[i].outputs = new Matrix(NETWORK_OUTPUT_SIZE, 1);
        players[i].inputs = new Matrix(NETWORK_INPUT_SIZE, 1);
        for (int j = 0; j < NETWORK_INPUT_SIZE; j++) {
            players[i].inputs->set(j, 0, 0);
        }
        for (int j = 0; j < NETWORK_OUTPUT_SIZE; j++) {
            players[i].outputs->set(j, 0, 0);
        }
    }
    if (this->logToFile) {
        csvOutputFile.open("game.csv");
        csvOutputFile << "STARTGAME, " << playerNumber / 2 << ","
                      << MAP_HEIGHT << "," << MAP_LENGTH << "," << BALL_SIZE
                      << "," << PLAYER_SIZE;
        for (int i = 0; i < wallNumber; i++) {
            csvOutputFile << "," << walls[i].pos.x << "," << walls[i].pos.y
                          << "," << walls[i].vitesse.x << ","
                          << walls[i].vitesse.y;
        }
        csvOutputFile << std::endl;
    }
}

Game::~Game() {
    if (logToFile) {
        csvOutputFile.close();
    }

    for (int i = 0; i < this->playerNumber; i++) {
        delete this->players[i].outputs;
        delete this->players[i].inputs;
    }

    delete[] wallsBouts;
    delete[] players;
    delete[] walls;
    delete[] goals;
}

/*
    Répartis les joueurs sur le terrain selon la configuration voulue.
    Pour 3 joueurs, on peut par exemple avoir {2, 1}, placés comme au vrai
    foot
*/
void Game::setup_kickoff(const int conf[], int n) {
    int c = 0, s = 0;
    double spx = (double)MAP_LENGTH / 2. / (double)(n + 1);

    for (int i = 0; i < n; i++) {
        s += conf[i];
    }

    if (2 * s != this->playerNumber)
        throw std::invalid_argument("invalid conf");

    for (int i = 0; i < n; i++) {
        double spy = MAP_HEIGHT / (double)(conf[i] + 1);

        for (int k = 1; k <= conf[i]; k++) {
            this->players[c].pos = {.x = (i + 1) * spx, .y = k * spy};
            this->players[c].vitesse = {.x = 0.01, .y = 0};
            this->players[c].orientation = random_double(-M_PI, M_PI);

            c++;
        }
    }

    for (int i = 0; i < n; i++) {
        for (int k = conf[i]; k >= 1; k--) {
            // symétrie centrale par rapport au centre du terrain
            vector centre = {.x = MAP_LENGTH / 2., .y = MAP_HEIGHT / 2.};
            this->players[c].pos =
                players[c - s].pos + 2 * (centre - players[c - s].pos);
            this->players[c].vitesse = {.x = -0.01, .y = 0};

            this->players[c].orientation =
                this->players[c - s].orientation + M_PI;
            c++;
        }
    }

    for (int i = 0; i < playerNumber; i++) {
        players[i].shootCooldown = 0;
    }

    /*this->ball.pos.x = this->players[1].pos.x;
    this->ball.pos.y = this->players[1].pos.y;
    this->ball.pos.x += ((float)PLAYER_SIZE + BALL_SIZE) / 2 + 30;*/
}

inline collisionList *insert(collisionList *list, ball *actor,
                             ball *secondary, int id1, int id2,
                             CollisionType type, double time = INFINITY) {
    collisionList *l = new collisionList;
    l->actor = actor;
    l->secondary = secondary;
    l->time = time;
    l->type = type;
    l->next = list;
    l->id1 = id1;
    l->id2 = id2;
    return l;
}
// Retourne le temps avant lequel un objet va rencontrer un mur
// (potentiellement négatif), ou NaN si la collision n'arrivera jamais
double get_wall_collision_time(ball *obj, ball *wall) {
    vector MO = obj->pos - wall->pos;

    vector n = {
        .x = -wall->vitesse.y,
        .y = wall->vitesse.x,
    };

    double c = abs(dot_product(MO, n) / norme(n));
    double ev = abs(dot_product(obj->vitesse, n) / norme(n));
    double T = c - obj->size;

    return T / ev;
}

// Retourne le temps avant lequel un objet va rencontrer un autre objet
// (potentiellement négatif), ou NaN si la collision n'arrivera jamais
double get_two_ball_collision_time(ball *b1, ball *b2) {
    vector v_relative = b1->vitesse - b2->vitesse;
    vector LM = b1->pos - b2->pos;
    double d = b1->size + b2->size;

    vector n = {
        .x = -v_relative.y,
        .y = v_relative.x,
    };

    double c = abs(dot_product(LM, n) / norme(n));
    double ev = abs(dot_product(LM, v_relative) / norme(v_relative));
    double pprime = sqrt(pow(d, 2) - pow(c, 2));

    return (ev - pprime) / norme(v_relative);
}

inline double distance_carre(ball &p, const ball &b) {
    return norme_carre(p.pos - b.pos);
}

// Remplit une liste chainée avec les conflits au moment où elle est
// appellée
collisionList *
Game::get_object_collision_list(int objId, collisionList *listToAppend) {
    struct ball *selected;
    if (objId == -1) {
        selected = &ball;
    } else {
        selected = &(this->players[objId]);
    }

    for (int i = 0; i < wallNumber; i++) {
        wall &w = this->walls[i];
        // on regarde la distance au mur, si elle est inférieure au rayon de
        // la balle, c'est que la balle est en collision
        vector normal = {.x = -w.vitesse.y, .y = w.vitesse.x};
        double longueurDuMur = norme(w.vitesse);
        double d =
            dot_product(selected->pos - w.pos, normal) / norme(normal);

        double pRelat = dot_product(selected->pos - w.pos, w.vitesse) /
                        (longueurDuMur * longueurDuMur);

        if (!(abs(d) < selected->size)) {
            // le joueur n'est pas dans la droite du mur
            continue;
        }
        if (abs(d) < selected->size && 0 < pRelat && 1 > pRelat) {
            // la balle est trop près du mur, il y a collision
            listToAppend =
                insert(listToAppend, selected, &w, objId, i, WALL);
        }
    }

    for (int i = 0; i < 2 * wallNumber; i++) {
        if (distance_carre(*selected, this->wallsBouts[i]) <
            pow(selected->size, 2)) {
            listToAppend =
                insert(listToAppend, selected, &this->wallsBouts[i], objId,
                       i + TEAM_SIZE * 2, CIRCLE);
        }
    }

    // test de collisions triangulaire balle-joueur ou joueur-joueur
    for (int i = objId + 1; i < playerNumber; i++) {
        if (distance_carre(*selected, this->players[i]) <
            pow(selected->size + PLAYER_SIZE, 2)) {
            listToAppend = insert(listToAppend, selected, &this->players[i],
                                  objId, i, CIRCLE);
        }
    }

    return listToAppend;
}

// Calcule l'instant (exact) de choc pour chaque collision de la liste
void time_collision_list(collisionList *collision) {
    if (!collision) {
        return;
    }
    if (collision->type == WALL) {
        collision->time =
            get_wall_collision_time(collision->actor, collision->secondary);
    } else {
        collision->time = get_two_ball_collision_time(collision->actor,
                                                      collision->secondary);
    }
    time_collision_list(collision->next);
}

// Effectue une collision entre obj1 et obj2, modifiant ainsi leurs vitesses
// /!\ Il ne faut appeler cette fonction qu'avec des objets se touchant, et
// ne le faire qu'une fois sinon le comportement est indéfini
void compute_collision_circle(ball *obj1, ball *obj2) {
    vector x1 = obj1->pos;
    double &m1 = obj1->mass;
    vector &v1 = obj1->vitesse;

    vector x2 = obj2->pos;
    vector &v2 = obj2->vitesse;
    double &m2 = obj2->mass;

    auto dv1 = (x1 - x2) *
               (dot_product(v1 - v2, x1 - x2) / norme_carre(x1 - x2)) *
               ((double)(2 * m2) / (m1 + m2));
    auto dv2 = (x2 - x1) *
               (dot_product(v2 - v1, x2 - x1) / norme_carre(x2 - x1)) *
               ((double)(2 * m1) / (m1 + m2));

    v1 = v1 - dv1 * COLLISION_CONS;
    v2 = v2 - dv2 * COLLISION_CONS;
}

// Comme au dessus, mais avec un mur
void compute_collision_wall(ball &b, ball *w) {
    vector um = w->vitesse / norme(w->vitesse);
    vector normal_um = {.x = um.y, .y = -um.x};

    b.vitesse = dot_product(-b.vitesse, normal_um) * normal_um +
                dot_product(b.vitesse, um) * um;
}

// Retourne un pointeur vers la premiere collision arrivant dans la liste,
// d'un point de vue temporel
collisionList *find_first_collision(collisionList *list) {
    if (!list)
        return NULL;

    switch (list->type) {
    case CIRCLE:
        list->time =
            get_two_ball_collision_time(list->actor, list->secondary);
        break;
    case WALL:
        list->time = get_wall_collision_time(list->actor, list->secondary);
        break;
    }

    auto nextBest = find_first_collision(list->next);
    if (nextBest && nextBest->time < list->time && list->time > 0) {
        return nextBest;
    }
    return NULL;
}

// Libere la liste chainée
void free_collision_list(collisionList *list) {
    if (list == NULL) {
        return;
    }
    free_collision_list(list->next);
    delete[] list;
}

void Game::move_all_obj(double time) {
    ball.pos += ball.vitesse * time;
    for (int i = 0; i < playerNumber; i++) {
        players[i].pos += players[i].vitesse * time;
        players[i].orientation += players[i].rvitesse * time;
    }
}

void Game::tick(double timeToAdvance, bool root, bool clearAccels,
                bool canSave) {
    // ici, root est un bouléen défini à vrai ssi c'est le premier appel
    // récursif
    if (root) {
        if (logToFile && canSave) {
            while (timeSinceLastSave >= 1) {
                write_players();
                timeSinceLastSave -= 1;
            }
        }

        timeSinceLastSave += timeToAdvance;
        apply_friction(timeToAdvance);

        execute_player_actions(timeToAdvance, clearAccels);
    }
    // On fait tout avancer de la durée voulue
    move_all_obj(timeToAdvance);

    // On obtient la liste des collisions APRÈS avoir bougé
    collisionList *c = NULL;

    for (int i = 0; i < playerNumber + 1; i++) {
        c = get_object_collision_list(i - 1, c);
    }
    // On regarde s'il y a des collisions
    if (c != NULL) {
        // S'il y a des collisions, c'est qu'il faut faire du backtracking
        // pour retrouver la réalité, donc on commence par retourner en
        // arrière
        move_all_obj(-timeToAdvance);
        this->infos.collisions += 1;

        // On regarde à quel instant est survenu la première collision
        collisionList *firstCollision = find_first_collision(c);

        // On avance à cet instant précis
        move_all_obj(firstCollision->time);
        // On effectue la collision
        switch (firstCollision->type) {
        case CIRCLE:
            if (firstCollision->id1 == -1 &&
                firstCollision->id2 < 2 * TEAM_SIZE) {
                this->infos.touchMean /= 2;
                if (firstCollision->id2 < TEAM_SIZE) {
                    this->infos.bonusBleu +=
                        (0.5 - this->infos.bonusBleu) / 3;
                    this->infos.touchMean += 0.5;
                } else {
                    this->infos.bonusRouge +=
                        (0.5 - this->infos.bonusRouge) / 3;
                    this->infos.touchMean -= 0.5;
                }
            }

            compute_collision_circle(firstCollision->actor,
                                     firstCollision->secondary);

            if (firstCollision->id1 == -1) {
                this->infos.ballCollisions += 1;
            }
            break;
        case WALL:
            compute_collision_wall(*firstCollision->actor,
                                   firstCollision->secondary);
            break;
        }
        // On fait de nouveau un tick, pour compléter le temps restant
        tick(timeToAdvance - firstCollision->time, false);
    }

    // On libère la liste dont on n'aura plus besoin
    free_collision_list(c);
};

void Game::write_players() {
    csvOutputFile << "2," << (double)this->ball.pos.x << ","
                  << (double)this->ball.pos.y << ",";
    for (int i = 0; i < playerNumber; i++) {
        csvOutputFile << (double)this->players[i].orientation << ","
                      << this->players[i].pos.x << ","
                      << this->players[i].pos.y << ",";
        for (int j = 0; j < NETWORK_INPUT_SIZE; j++) {
            csvOutputFile << (double)(this->players[i].inputs->get(j, 0))
                          << ",";
        }
        for (int j = 0; j < NETWORK_OUTPUT_SIZE; j++) {
            csvOutputFile << (double)(this->players[i].outputs->get(j, 0))
                          << ",";
        }
    }
    csvOutputFile << std::endl;
}

// Effectue une action pour un joueur donné (ce qui lui permet de tourner
// et/ou d'accélérer)
void Game::set_accelerations(unsigned int id, double rotation,
                             double acceleration) {
    players[id].raccel = rotation * PLAYER_ROTATION_ACCELERATION;
    players[id].acceleration = acceleration * PLAYER_ACCELERATION;
};

void Game::execute_player_actions(double time, bool clearAccels) {
    for (int id = 0; id < playerNumber; id++) {
        player &selected = players[id];
        selected.rvitesse += selected.raccel * time;
        selected.vitesse.x +=
            selected.acceleration * cos(players[id].orientation) * time;
        selected.vitesse.y +=
            selected.acceleration * sin(players[id].orientation) * time;
        if (clearAccels) {
            selected.raccel = 0;
            selected.acceleration = 0;
        }
    }
}

void Game::apply_friction(double time) {
    // On applique les frottements
    // Ces frottements sont des frottements fluides (F = K*v)
    ball.vitesse -= (ball.vitesse * BALL_FRICTION) * time;

    for (int i = 0; i < playerNumber; i++) {
        players[i].vitesse -= (players[i].vitesse * PLAYER_FRICTION) * time;
        players[i].rvitesse -=
            (players[i].rvitesse * PLAYER_ROTATION_FRICTION) * time;
    }
}

void Game::set_ball(vector pos, vector vitesse, double size, double mass) {
    this->ball = {
        .pos = pos, .vitesse = vitesse, .size = size, .mass = mass};
}

void Game::set_player(int id, vector pos, vector speed, double orientation,
                      double size, double mass) {
    this->players[id].pos = pos;
    this->players[id].vitesse = speed;
    this->players[id].orientation = angle_rounded(orientation);
    this->players[id].size = size;
    this->players[id].acceleration = 0;
    this->players[id].raccel = 0;
    this->players[id].mass = mass;
}

bool Game::check_goal(int id) {
    double cage = get_wall_collision_time(&this->ball, &this->goals[id]);

    if (cage < 0 || cage > 1)
        return false;

    auto dist = this->ball.pos - goals[id].pos;

    auto vertical = dot_product(this->goals[id].vitesse, dist) /
                    norme(this->goals[id].vitesse);

    if (0 < vertical && vertical < GOAL_HEIGHT) {
        return true;
    }

    return false;
}

/*
    La valeur de g.infos.score est positive lorsque c1 remporte la partie,
    négative dans le cas contraire. c1 est placé à gauche du terrain tandis
   que c2 à droite.
*/
gameInformations play_match(Chromosome *c1, Chromosome *c2, bool save) {
    auto g = Game(2 * TEAM_SIZE, save);
    g.ball.pos = {.x = MAP_LENGTH / 2., .y = MAP_HEIGHT / 2.};
    g.ball.vitesse = {.x = 0, .y = 0};

    int c[] = GAME_CONFIG;
    g.setup_kickoff(c, GAME_CONFIG_LENGTH);

    int to_touch = MAX_TOUCH_DURATION;
    unsigned int deltaTouchedBall = 0;

    int k;
    for (k = 0; k < MAX_GAME_DURATION; k++, to_touch--) {
        c1->collect_and_apply(g.players, g.players + TEAM_SIZE, &g.ball,
                              false);
        c2->collect_and_apply(g.players + TEAM_SIZE, g.players, &g.ball,
                              true);

        for (int a = 0; a < 2 * TEAM_SIZE; a++) {
            double rotation = g.players[a].outputs->get(0, 0);
            double acceleration = g.players[a].outputs->get(1, 0);
            double shoot = g.players[a].outputs->get(2, 0);

            if (acceleration < 0)
                acceleration = 0;

            g.set_accelerations(a, rotation, acceleration);

            if ((shoot >= 0) && (g.players[a].shootCooldown == 0)) {
                g.players[a].shootCooldown = SHOOT_COOLDOWN;

                auto explo = (g.ball.pos - g.players[a].pos);
                double d = norme(explo);
                // vecteur unitaire de direction de projection
                explo /= d;
                // on obient la norme de l'accélération causée par
                // "l'explosion"
                double dmax = PLAYER_SIZE + 2 * BALL_SIZE;
                explo *= fmax(dmax - d, 0) / dmax * 60;

                g.ball.vitesse += explo;
            }
            if (shoot < 0) {
                g.players[a].shootCooldown =
                    fmax(g.players[a].shootCooldown - 1, 0);
            }
        }

        g.tick(1);

        bool bc1 = g.check_goal(0);
        bool bc2 = g.check_goal(1);
        if (bc1 || bc2) {
            if (bc1) {
                c2->stats.instanceGoals += 1;
                // but contre leur camp des bleus
                if (g.infos.touchMean > 0.75) {
                    g.infos.scoreBleu -= 1;
                } else {
                    g.infos.scoreRouge += 1;
                }
            } else {
                c1->stats.instanceGoals += 1;
                if (-g.infos.touchMean > 0.75) {
                    g.infos.scoreRouge -= 1;
                } else {
                    g.infos.scoreBleu += 1;
                }
            }
            g.infos.goals += 1;

            g.ball.pos.x = (float)MAP_LENGTH / 2;
            g.ball.pos.y = (float)MAP_HEIGHT / 2;
            g.ball.vitesse.x = 0;
            g.ball.vitesse.y = 0;

            g.setup_kickoff(c, GAME_CONFIG_LENGTH);
        }

        if (to_touch == 0) {
            if (g.infos.ballCollisions - deltaTouchedBall == 0) {
                g.infos.stopped = true;
                break;
            }
            deltaTouchedBall = g.infos.ballCollisions;

            to_touch = MAX_TOUCH_DURATION;
        }
    }

    vector centreCageBleu = {.x = static_cast<double>(0),
                             .y = (double)MAP_HEIGHT / 2};
    vector centreCageRouge = {.x = static_cast<double>(MAP_LENGTH),
                              .y = (double)MAP_HEIGHT / 2};
    // bonus de position
    double diffPosBalle = norme(centreCageBleu - g.ball.pos) -
                          norme(centreCageRouge - g.ball.pos);

    g.infos.scoreBleu += fmax(diffPosBalle / MAP_LENGTH, 0);
    g.infos.scoreRouge += fmax(-diffPosBalle / MAP_LENGTH, 0);
    g.infos.scoreBleu += g.infos.bonusBleu;
    g.infos.scoreRouge += g.infos.bonusRouge;

    return g.infos;
};
