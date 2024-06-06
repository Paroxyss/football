#include <iostream>
#include <ostream>
#include <stdexcept>

#include "Activation.hpp"
#include "Chromosome.hpp"
#include "Game.hpp"
#include "Genealogy.hpp"
#include "Inputs.hpp"
#include "Matrix.h"
#include "config.h"
#include "util.hpp"

Chromosome::Chromosome() {
	this->id = uid();
	for (int i = 0; i < EQUIPE_SIZE; i++) {
		for (int j = 0; j < NETWORK_SIZE - 1; j++) {
			this->matrix[i][j] =
				new Matrix(PLAYER_LAYERS[j + 1], PLAYER_LAYERS[j]);
		}
	}
	for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
		this->didier[i] =
			new Matrix(DIDIER_LAYERS[i + 1], DIDIER_LAYERS[i]);
	}
}

Chromosome::~Chromosome() {
	for (int i = 0; i < EQUIPE_SIZE; i++) {
		for (int j = 0; j < NETWORK_SIZE - 1; j++) {
			delete this->matrix[i][j];
		}
	}
	for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
		delete this->didier[i];
	}
}

void Chromosome::print() {
	std::cout << "Chromosome, " << EQUIPE_SIZE << " joueurs" << std::endl;
	for (int i = 0; i < EQUIPE_SIZE; i++) {
		std::cout << "Joueur " << i << std::endl;
		for (int j = 0; j < NETWORK_SIZE - 1; j++) {
			this->matrix[i][j]->print();
		}
	}
	std::cout << "Didier: " << std::endl;
	for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
		this->didier[i]->print();
	}
}

void Chromosome::initialize() {
	for (int i = 0; i < EQUIPE_SIZE; i++) {
		for (int j = 0; j < NETWORK_SIZE - 1; j++) {
			this->matrix[i][j]->initialize();
		}
	}
	this->hasDidier = likelyness(0.5);
	for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
		this->didier[i]->initialize();
	}
}

void Chromosome::apply(player *equipeAlliee) {
	for (int i = 0; i < EQUIPE_SIZE; i++) {
		player &selected = equipeAlliee[i];

		// Cette matrice va servir à faire le calcul en place
		Matrix mcalcul = Matrix(NETWORK_INPUT_SIZE, 1);

		// On copie la matrice d'input dans celle de calcul
		for (int j = 0; j < NETWORK_INPUT_SIZE; j++) {
			mcalcul.set(j, 0, selected.inputs->get(j, 0));
		}

		mcalcul.mult_inv(*this->matrix[i][0]);
		input_layer_activation(mcalcul);

		for (int j = 1; j < NETWORK_SIZE - 2; j++) {
			mcalcul.mult_inv(*this->matrix[i][j]);
			hidden_layer_activation(mcalcul);
		}

		mcalcul.mult_inv(*this->matrix[i][NETWORK_SIZE - 2]);
		output_layer_activation(mcalcul);

		for (int j = 0; j < NETWORK_OUTPUT_SIZE; j++) {
			selected.outputs->set(j, 0, mcalcul.get(j, 0));
		}
	}
}

void Chromosome::apply_didier(player *equipeAlliee) {
	Matrix inputs = Matrix(COM_SIZE * EQUIPE_SIZE, 1);

	for (int i = 0; i < EQUIPE_SIZE; i++) {
		for (int j = 0; j < COM_SIZE; j++) {
			inputs.set(i * COM_SIZE + j, 0,
					   equipeAlliee[i].outputs->get(
						   NETWORK_OUTPUT_SIZE - COM_SIZE + j, 0));
		}
	}

	for (int i = 1; i < DIDIER_NETWORK_SIZE - 2; i++) {
		inputs.mult_inv(*this->didier[i]);
		hidden_layer_activation(inputs);
	}

	inputs.mult_inv(*this->didier[DIDIER_NETWORK_SIZE - 2]);
	output_layer_activation(inputs);
	
	for (int i = 0; i < EQUIPE_SIZE; i++) {
		for (int j = 0; j < COM_SIZE; j++) {
			equipeAlliee[i].inputs->set(j, 0,
										inputs.get(i * COM_SIZE + j, 0));
		}
	}
}

void Chromosome::collect_and_apply(player *equipeAlliee,
								   player *equipeAdverse, ball *b,
								   bool team) {
	if (this->hasDidier) {
		apply_didier(equipeAlliee);
	} else {
		for (int i = 0; i < EQUIPE_SIZE; i++) {
			for (int j = 0; j < COM_SIZE; j++) {
				equipeAlliee[i].inputs->set(j, 0, 0);
			}
		}
	}
	// On sauvegarde les inputs dans les joueurs
	for (int i = 0; i < EQUIPE_SIZE; i++) {
		writeInputs(equipeAlliee[i], equipeAlliee, equipeAdverse, b, team);
	}

	// Evaluation du réseau de neurones de chaque joueurs.
	this->apply(equipeAlliee);
}

void Chromosome::write(std::ofstream &file) {
	int equipeSize = EQUIPE_SIZE;
	int nSize = NETWORK_SIZE;
	int didierSize = DIDIER_NETWORK_SIZE;

	// Par sécurité
	file.write((char *)&equipeSize, sizeof(int));
	file.write((char *)&nSize, sizeof(int));
	file.write((char *)&didierSize, sizeof(int));

	// Configs
	for (int i = 0; i < NETWORK_SIZE; i++) {
		file.write((char *)&PLAYER_LAYERS[i], sizeof(int));
	}
	for (int i = 0; i < DIDIER_NETWORK_SIZE; i++) {
		file.write((char *)&DIDIER_LAYERS[i], sizeof(int));
	}

	// Stats
	file.write((char *)&this->stats.instanceAge, sizeof(int));
	file.write((char *)&this->stats.instanceGoals, sizeof(int));
	file.write((char *)&this->hasDidier, sizeof(bool));

	for (int i = 0; i < EQUIPE_SIZE; i++) {
		for (int j = 0; j < NETWORK_SIZE - 1; j++) {
			this->matrix[i][j]->write(file);
		}
	}
	for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
		this->didier[i]->write(file);
	}
}

Chromosome *Chromosome::read(std::ifstream &file) {
	int equipeSize;
	int nSize;
	int didierSize;

	// Par sécurité
	file.read((char *)&equipeSize, sizeof(int));
	file.read((char *)&nSize, sizeof(int));
	file.read((char *)&didierSize, sizeof(int));

	if (equipeSize != EQUIPE_SIZE || nSize != NETWORK_SIZE ||
		didierSize != DIDIER_NETWORK_SIZE) {
		std::cout << equipeSize << "≠" << EQUIPE_SIZE << " || " << nSize
				  << "≠" << NETWORK_SIZE << " || " << didierSize << "≠"
				  << DIDIER_NETWORK_SIZE << std::endl;
		throw std::invalid_argument(
			"Misconfigured Chromosome file (bad constants)");
	}

	const int pLayers[NETWORK_SIZE] = {};
	const int pDidierLayers[DIDIER_NETWORK_SIZE] = {};

	// Configs
	for (int i = 0; i < NETWORK_SIZE; i++) {
		file.read((char *)&pLayers[i], sizeof(int));
		if (pLayers[i] != PLAYER_LAYERS[i]) {
			std::cout << i << ":" << pLayers[i] << " ≠ "
					  << PLAYER_LAYERS[i];
			throw std::invalid_argument(
				"Misconfigured Chromosome file (bad player layers)");
		}
	}
	for (int i = 0; i < DIDIER_NETWORK_SIZE; i++) {
		file.read((char *)&pDidierLayers[i], sizeof(int));
		if (pDidierLayers[i] != DIDIER_LAYERS[i]) {
			std::cout << i << ":" << pDidierLayers[i] << " ≠ "
					  << DIDIER_LAYERS[i];
			throw std::invalid_argument(
				"Misconfigured Chromosome file (bad didier layers)");
		}
	}

	Chromosome *c = new Chromosome();

	// Stats
	file.read((char *)&c->stats.instanceAge, sizeof(int));
	file.read((char *)&c->stats.instanceGoals, sizeof(int));
	file.read((char *)&c->hasDidier, sizeof(bool));

	for (int i = 0; i < EQUIPE_SIZE; i++) {
		for (int j = 0; j < NETWORK_SIZE - 1; j++) {
			delete c->matrix[i][j];
			c->matrix[i][j] = Matrix::read(file);
		}
	}

	for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
		delete c->didier[i];
		c->didier[i] = Matrix::read(file);
	}

	return c;
}
