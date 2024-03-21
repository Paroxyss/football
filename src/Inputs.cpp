#include <cmath>

#include "Game.hpp"
#include "Inputs.hpp"
#include "Matrix.h"
#include "Vector.hpp"
#include "config.h"
/**
 * @brief min-max normalization: Retranche la valeur donnée entre 0 et 1.
 */

// Opérations couteuses donc mises en statiques, elles sont utilisées
// pour la normalisation min-max.
static const double d_map = sqrt(pow(MAP_LENGTH, 2) + pow(MAP_HEIGHT, 2));
static const double d_centre_cage =
    sqrt(pow(MAP_HEIGHT / 2, 2) + pow(MAP_LENGTH, 2));

// Vitesse maximum du joueur
static const double vjmax = PLAYER_ACCELERATION / PLAYER_FROTTEMENT;
// Vitesse maximum de la balle
static const double vbmax = vjmax * sqrt(PLAYER_MASS / BALL_MASS);

static const double vrjmax =
    PLAYER_ROTATION_ACCELERATION / PLAYER_ROTATION_FROTTEMENT;

double mmn(double x, double min, double max) {
    return (x - min) / (max - min);
}
double mmn_negatif(double x, double min, double max) {
    return mmn(x, min, max) * 2 - 1;
}
double normalizeAngle(double a) {
    return mmn_negatif(a, -M_PI, M_PI);
}

// fonctions qui écrivent dans la matrices d'entrée un certain objet

// cout : 1 entrée
inline void writeValRaw(Matrix &mat, double v, uint &i) {
    mat.set(i, 0, v);
    i++;
}

// cout : 1 entrée
inline void writeVal(Matrix &mat, double v, uint &i, double min, double max) {
    mat.set(i, 0, mmn(v, min, max));
    i++;
}

inline void writeValn(Matrix &mat, double v, uint &i, double min, double max) {
    mat.set(i, 0, mmn_negatif(v, min, max));
    i++;
}

// cout : 2 entrées
inline void writeAngle(Matrix &mat, double angle, uint &i) {
    // déjà entre -1 et 1
    writeValRaw(mat, sin(angle), i);
    writeValRaw(mat, cos(angle), i);
}

// cout : 3 entrées
inline void writeVector(Matrix &mat, player &viewer, vector vec, uint &i,
                        double maxNorme) {
    double nv = norme(vec);
    writeVal(mat, nv, i, 0, maxNorme);
    writeAngle(mat, nv == 0 ? 0 : vangle(vec) - viewer.orientation, i);
}

// cout : 6 entrées
inline void writeObject(Matrix &mat, player &viewer, ball &target, uint &i,
                        double vMax) {
    writeVector(mat, viewer, target.pos - viewer.pos, i, d_map);
    writeVector(mat, viewer, target.vitesse - viewer.vitesse, i, vMax);
}

// Écrit le joueur le plus proche de l'équipe spécifiée
// cout : 6 entrées
inline void writeNearestPlayer(Matrix &mat, player &viewer, player *&equipe,
                               uint &i) {
    player *nearest = &viewer;
    double d = INFINITY;

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        double nd = norme(equipe[i].pos - viewer.pos);
        if (nd < d && nd > PLAYER_SIZE / 2.) {
            nearest = &equipe[i];
            d = nd;
        };
    }
    // 2*vjmax car les joueurs peuvent aller à vitesse max en s'éloignant
    writeObject(mat, viewer, *nearest, i, 2 * vjmax);
}

/*
    team == false -> gauche
    team == true -> droite
*/
void writeInputs(player &viewer, player *equipeAlliee, player *equipeAdverse,
                 ball *b, bool team) {
    Matrix &mat = *viewer.inputs;
    uint indice = 0;

    for (int i = 0; i < COM_SIZE; i++) {
        writeValRaw(mat, viewer.inputs->get(i, 0), indice);
    }

    writeValn(mat, viewer.rvitesse, indice, -vrjmax, vrjmax);
    // vitesse du joueur
    writeVector(mat, viewer, viewer.vitesse, indice, vjmax);

    vector centreCage = {.x = static_cast<double>(team ? 0 : MAP_LENGTH),
                         .y = (double)MAP_HEIGHT / 2};

    vector vCage = centreCage - viewer.pos;
    // position de la cage adverse
    writeVector(mat, viewer, vCage, indice, d_centre_cage);

    vector ex = {.x = 0, .y = 1};
    double h = dotProduct(ex, vCage);
    if (team)
        h = -h;
    // distance normale au milieu
    writeValn(mat, h, indice, -MAP_HEIGHT / 2., MAP_HEIGHT / 2.);

    // balle
    writeObject(mat, viewer, *b, indice, vbmax + vjmax);

    // Joueur le plus proche des équipes alliée et adverse
    writeNearestPlayer(mat, viewer, equipeAlliee, indice);
    writeNearestPlayer(mat, viewer, equipeAdverse, indice);

#ifdef MATRIX_DEBUG
    if (indice != NETWORK_INPUT_SIZE) {
        std::cout << indice << std::endl;
        throw std::logic_error("mauvais nombre d'inputs??");
    }
#endif
}
