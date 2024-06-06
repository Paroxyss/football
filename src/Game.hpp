#pragma once

#include <fstream>

#include "Matrix.hpp"
#include "Vector.hpp"
#include "config.hpp"

struct ball {
	// pos en m, vitesse en m.s-1
	vector pos, vitesse;
	// size le rayon en m, mass en kg
	double size, mass;
};

struct player : ball {
	// raccel en rad.s-2, rvitesse en rad.s-1, orientation en radian,
	// acceleration en m.s-2
	double raccel, rvitesse, orientation, acceleration;
	// Contient les cannaux de communiactions des joueurs pour didier pour
	// le tick suivant. Au début de la partie, ils valent 0;
	Matrix *outputs;
	// Contient les inputs que didier définit au début de l'évaluation, ils
	// valent par défaut 0
	Matrix *inputs;
	// Capacité d'un joueur à tirer
	int shootCooldown = 0;
};

// faut la mettre ici sinon ça compile pas (mdr)
#include "Chromosome.hpp"

// il faut le me mettre ici sinon ça complie pas (mdr)
class Chromosome;

// alors oui c'est absolument honteux, mais le polymorphisme c'est la méga
// fête. pos est le points d'origine, vitesse le vecteur directeur, et size
// pourra déterminer la longueur
struct wall : ball {};

enum CollisionType { WALL, CIRCLE };

struct collisionList {
	// temps auquel la collision arrive (en s)
	double time;
	// Acteur de la collision
	int id1;
	ball *actor;
	// Objet la subissant
	int id2;
	ball *secondary;
	CollisionType type;
	collisionList *next;
};

extern std::ofstream csvOutputFile;

class Game {
  public:
	bool logToFile;
	double timeSinceLastSave = 1;

	player *players;
	unsigned int playerNumber;
	ball ball;

	unsigned int wallNumber;
	wall *walls;
	struct ball *wallsBouts;
	wall *goals;

	gameInformations infos = {.collisions = 0,
							  .goals = 0,
							  .ballCollisions = 0,
							  .scoreRouge = 0,
							  .scoreBleu = 0,
							  .stopped = false,
							  .touchMean = 0,
							  .bonusRouge = 0,
							  .bonusBleu = 0};

    // Postionne les joueurs en position d'engagement
	void setup_kickoff(const int conf[], int n);

	Game(int playerNumber, bool logToFile = false);
	~Game();

	// fait avancer la simulation du temps souhaité
	void tick(double timeToAdvance = 1, bool root = true,
			  bool clearAccels = true, bool canSave = true);

    // Appliquer les accélérations liées aux décisions du joueur
	void set_accelerations(unsigned int id, double rotation,
                           double acceleration);
	// Ces fonctions dépendent du temps car les accélérations sont en x.s-2,
	// on doit donc par exemple les doubler si on veut appliquer
	// l'équivalent de leurs effets sur 2 secondes
	void execute_player_actions(double time, bool clearAccels);

    // Applique les accélérations liées aux forces de friction pour un temps donné
    void apply_friction(double time);

	// Définit les attributs de la balle
    void set_ball(vector pos, vector vitesse = {.x = 0, .y = 0},
                  double size = BALL_SIZE, double mass = BALL_MASS);

	// liste les collisions futures d'un objet
	collisionList *
	get_object_collision_list(int objId, collisionList *listToAppend = NULL);

    // Regarde si la balle est dans la cage {id}, ou alors qu'elle va traverser la ligne dans la seconde suivante
    bool check_goal(int id);

	// Déplace tous les objets pour la durée donnée (en s)
	void move_all_obj(double time);

    // Écrit l'état des joueurs dans le fichier csv (si la sauvegarde est activée)
	void write_players();

    // Définit les attributs d'un joueur
	void set_player(int id, vector pos, vector speed, double orientation,
                    double size = PLAYER_SIZE, double mass = PLAYER_MASS);
};

gameInformations play_match(Chromosome *c1, Chromosome *c2,
							bool save = false);
