#include "config.hpp"
#include "Game.hpp"
#include "Crossover.hpp"
#include "Chromosome.hpp"
#include "Matrix.hpp"

#include "util.hpp"

#include <stdexcept>

Matrix *one_pointer_crossover(Matrix &a, Matrix &b) {
	if (a.ligne != b.ligne || a.col != b.col)
		throw std::invalid_argument("Dimensions invalides opc");

	int x = rand() % a.col;
	Matrix *c = new Matrix(a.ligne, a.col);

	for (int j = 0; j < a.col; j++) {
		Matrix &t = j < x ? a : b;

		for (int i = 0; i < a.ligne; i++) {
			c->set(i, j, t.get(i, j));
		}
	}

	return c;
}

Matrix *uniform_crossover(Matrix &a, Matrix &b) {
	if (a.ligne != b.ligne || a.col != b.col)
		throw std::invalid_argument("Dimensions invalides uc");

	Matrix *c = new Matrix(a.ligne, a.col);

	for (int i = 0; i < a.ligne; i++) {
		for (int j = 0; j < a.col; j++) {
			double x = (likelyness(0.5) ? a : b).get(i, j);
			c->set(i, j, x);
		}
	}

	return c;
}

Chromosome *classicCrossover(Chromosome &a, Chromosome &b) {
	Chromosome *child = new Chromosome();
	child->hasDidier = likelyness(0.5 * (a.hasDidier + b.hasDidier));

	for (int k = 0; k < EQUIPE_SIZE; k++) {
		for (int i = 0; i < NETWORK_SIZE - 1; i++) {

			Matrix *m = uniform_crossover(*a.matrix[k][i], *b.matrix[k][i]);

			for (int j = 0; j < m->ligne; j++) {
				for (int l = 0; l < m->col; l++) {
					child->matrix[k][i]->set(j, l, m->get(j, l));
				}
			}

			delete m;
		}
	}

	int nDidier = a.hasDidier + b.hasDidier;

	switch (nDidier) {
	case 0:
		break;
	case 2:
		for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
			Matrix *m = uniform_crossover(*a.didier[i], *b.didier[i]);

			for (int j = 0; j < m->ligne; j++) {
				for (int l = 0; l < m->col; l++) {
					child->didier[i]->set(j, l, m->get(j, l));
				}
			}
		}
		break;
	case 1:
		Matrix **source = b.didier;
		if (a.hasDidier) {
			source = a.didier;
		}
		for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
			Matrix::clone(source[i], child->didier[i]);
		}
		break;
	}

	return child;
}

Chromosome *swapPlayerCrossover(Chromosome &a, Chromosome &b) {
	Chromosome *child = new Chromosome();

	child->hasDidier = likelyness(0.5 * (a.hasDidier + b.hasDidier));

	for (int k = 0; k < EQUIPE_SIZE; k++) {
		Chromosome &source = likelyness(0.5) ? a : b;
		for (int i = 0; i < NETWORK_SIZE - 1; i++) {
			Matrix::clone(source.matrix[k][i], child->matrix[k][i]);
		}
	}
	return child;
}

Chromosome *crossover(Chromosome &a, Chromosome &b) {
    if (likelyness(SWAP_CROSSOVER_PROBA)) {
        return swapPlayerCrossover(a, b);
    }
    return classicCrossover(a, b);
}
