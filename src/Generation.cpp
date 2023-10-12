#include "Generation.hpp"
#include "util.hpp"

Generation::Generation(unsigned int nthread){
	this->nthread = nthread;
}
Generation::~Generation(){
	delete this->currentPop;
}

void Generation::createPopulation(unsigned int size) {
    Population *pop = new Population(size);
    pop->initialize();
    delete this->currentPop;
    this->currentPop = pop;
    this->arbre.couchesSize = currentPop->size;
	arbre.ajouteCouche();
	for(int i = 0; i < size; i++){
		arbre.pushId(this->currentPop->pop[i]->id, 0, 0);
	}
}

void Generation::appendStatsFile(gameStatistics g, int forceGen) {
    if (forceGen == -1) {
        forceGen = generation;
    }
    statsFile << forceGen << ", " << g.totalCollisions / g.n << ", "
              << g.total_ball_collisions / g.n << ", " << g.totalGoals / g.n
              << ", " << ((double)g.stopped / g.n) * 100. << std::endl;
}

void Generation::rewriteStats() {
    if (statsFile.is_open()) {
        statsFile.close();
    }

    std::ofstream ofs;
    ofs.open("stats.csv", std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    // opening file using ofstream
    statsFile.open("stats.csv", std::ios::app);
    int i = 0;
    for (auto &s : stats) {
        appendStatsFile(s, i++);
    }
}

void Generation::step() {
    gameStatistics genStats = this->currentPop->next(nthread, false, this);
    stats.push_back(genStats);
    appendStatsFile(genStats);
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
void Generation::saveArbre(std ::ofstream &file) {
	arbre.writeJson(file);
}
