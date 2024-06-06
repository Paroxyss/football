#pragma once

#include <fstream>

#include "Game.hpp"
#include "Matrix.h"
#include "config.h"
#include "Rand.h"

typedef struct chromosomeStats {
	unsigned short instanceGoals;
	int instanceAge;
} chromosomeStats;

class Chromosome {
  public:
	chromosomeStats stats = {.instanceGoals = 0, .instanceAge = 0};
	unsigned long id;
	bool hasDidier = false;

	// Tableau des réseaux de neurones
	Matrix *matrix[EQUIPE_SIZE][NETWORK_SIZE - 1];
	// Réseau de communication
	Matrix *didier[DIDIER_NETWORK_SIZE - 1];

	Chromosome();
	~Chromosome();

	void print();

	// Initialise un chromosome avec des poids aléatoires (pour la
	// population initiale et le new blood)
	void initialize();

	/*
		Évaluation complète du chromosome:
			- Evaluation du rééseaux de communication
			- Agregation des valeurs d'entrées
			- Evaluation des résaux des joueurs
			- Ecriture des sorties dans le tableau de sortie
	*/
	void collect_and_apply(player *equipeAlliee, player *equipeAdverse,
						   ball *b, bool team);

	// Evalue les réseaux des joueurs
	void apply(player *equipeAlliee);

	/*
		Didier est toujours évalué avant les joueurs.
		Chaque joueur contient les outputs qu'il a donné à la fin du tick
		précédent. On les passe en entrée pour didier, en sortie, didier
		écrit lets inputs destinés aux joueurs dans leurs champ output
	*/
	void apply_didier(player *equipeAlliee);

	// Sauvegarde du chromosome dans un fichier binaire
	void write(std::ofstream &file);
	
	// Chargement du chromosome depuis un fichier binaire
	static Chromosome *read(std::ifstream &file);
};