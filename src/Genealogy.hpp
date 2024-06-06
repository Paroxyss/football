#pragma once

#include <fstream>
#include <list>
#include <vector>

#include "util.hpp"

typedef unsigned long cid_t;
typedef struct carteIdentite {
    cid_t id;
    cid_t p1;
    cid_t p2;
} carteIdentite;

unsigned long uid();

class CoucheGenealogique {
  private:
    std::vector<carteIdentite> ids;
    unsigned int expectedSize;

  public:
    CoucheGenealogique(unsigned int size);
    void resize(unsigned int size);

    void push_id(cid_t targetId, cid_t p1, cid_t p2);
    carteIdentite get(int index);

    OUTFILE_METHOD(writeJson)
    OUTFILE_METHOD(write)
    static void read(std::ifstream &file, CoucheGenealogique *dest);
};

class ArbreGenealogique {
  private:
    std::list<CoucheGenealogique> couches;

  public:
    unsigned int couchesSize;
    ArbreGenealogique(unsigned int couchesSize = 0);
    unsigned int size();

    // ajoute une identité à la derniere couche
    void push_id(cid_t targetId, cid_t p1, cid_t p2);
    void ajoute_couche();

    OUTFILE_METHOD(writeJson)
    OUTFILE_METHOD(write)
    static void read(std::ifstream &file, ArbreGenealogique *dest);
};
