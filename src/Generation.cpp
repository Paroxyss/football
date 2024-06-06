#include "Generation.hpp"
#include "util.hpp"

Generation::Generation(unsigned int nthread) {
	this->nthread = nthread;
	statsFile.open("stats.csv", std::ios::app);
}
Generation::~Generation() {
	delete this->currentPop;
}

void Generation::create_population(unsigned int size) {
	Population *pop = new Population(size);
	pop->initialize();
	delete this->currentPop;
	this->currentPop = pop;
	this->arbre.couchesSize = currentPop->size;
    arbre.ajoute_couche();
	for (int i = 0; i < size; i++) {
        arbre.push_id(this->currentPop->pop[i]->id, 0, 0);
	}
}

void push_stats_to_file(std::ofstream &f, gameStatistics g, int generation,
                        double propDidier) {
	f << "[" << generation << ", " << (double)g.totalCollisions / g.n
      << ", " << (double)g.totalBallCollisions / g.n << ", "
	  << (double)g.totalGoals / g.n << ", " << propDidier << "]";
}

void Generation::append_stats_file(gameStatistics g, int forceGen) {
	if (forceGen == -1) {
		forceGen = generation;
	}

	uint proportionDidier = 0;
	for (int i = 0; i < currentPop->size; i++) {
		proportionDidier += currentPop->pop[i]->hasDidier;
	}

    push_stats_to_file(statsFile, g, forceGen,
                       (double) proportionDidier / currentPop->size);
	statsFile << std::endl;
}

void Generation::rewrite_stats() {
	if (statsFile.is_open()) {
		statsFile.close();
	}

	// vider le fichier
	statsFile.open("stats.csv", std::ofstream::out | std::ofstream::trunc);
	statsFile.close();

	statsFile.open("stats.csv", std::ios::app);
	int i = 0;
	for (auto &s : stats) {
        append_stats_file(s, i++);
	}
}

void Generation::step() {
	gameStatistics genStats = this->currentPop->next(nthread, false, this);
	stats.push_back(genStats);
    append_stats_file(genStats);
	generation += 1;
}

// à la différence des autres classes, ici on peut charger et sauvegarder
// les données au sein d'une même instance
void Generation::save(std ::ofstream &file) {
	WRITE(generation);
	currentPop->write(file);
	arbre.write(file);
	unsigned int statsSize = stats.size();
	WRITE(statsSize);
	for (auto &s : stats) {
		WRITE(s);
	}
}
void Generation::load(std ::ifstream &file) {
	READ(generation);

	delete this->currentPop;
	currentPop = Population::read(file);
	ArbreGenealogique::read(file, &this->arbre);
	unsigned int statsSize;
	READ(statsSize);

	for (int i = 0; i < statsSize; i++) {
		gameStatistics s;
		READ(s);
		stats.push_back(s);
	}
	this->arbre.couchesSize = currentPop->size;
}
void Generation::saveJson(std ::ofstream &file) {
	file << "{\"genealogy\":";
	arbre.writeJson(file);
	file << ", \"stats\":[";
	int i = 0;

	for (auto &stat : stats) {
        push_stats_to_file(file, stat, i++, 0);
		std::cout << i << " " << generation << std::endl;
		if (i != generation) {
			file << ",";
		}
	}
	file << "]}";
}
