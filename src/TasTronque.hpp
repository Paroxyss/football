#pragma once

#include "Chromosome.hpp"

class TasSelection {
  public:
	int nComparaison = 0;
    Chromosome **tas;
    int virtualSize;
    int selectedInput;

  public:
    TasSelection(int virtualSize);

    void insere(Chromosome *c);
	void remonteInsertion(int id);
    Chromosome *extraire();
	void remonteExtraction(int id);

	void afficher();

	void echanger(int id1, int id2);
};
