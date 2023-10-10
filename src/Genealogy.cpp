#include <fstream>
#include <iostream>
#include <mutex>
#include <stdexcept>

#include "Genealogy.hpp"
#include "config.h"
#include "util.hpp"

std::mutex idMutex;
unsigned long id = 0;
unsigned long uid() {
    std::lock_guard<std::mutex> lock(idMutex);
    return ++id;
};

CoucheGenealogique::CoucheGenealogique(unsigned int size) {
    ids.reserve(size);
    expectedSize = size;
}

void CoucheGenealogique::resize(unsigned int size) {
    if (ids.size() > size) {
        throw std::logic_error(
            "Tentative de redimensionnement d'une couche trop pleine");
    }
    expectedSize = size;
    ids.reserve(size);
};

void CoucheGenealogique::pushId(cid_t targetId, cid_t p1, cid_t p2) {
    if (ids.size() == expectedSize) {
        throw std::logic_error(
            "Tentative d'insertion dans une couche généalogique pleine");
    }
    ids.push_back((carteIdentite){.id = targetId, .p1 = p1, .p2 = p2});
};

void CoucheGenealogique::writeJson(std::ofstream &file) {
    file << "[";
    for (int i = 0; i < ids.size(); i++) {
        auto id = ids[i];
        file << "[" << id.id << ", " << id.p1 << ", " << id.p2 << "]";
        if (i < ids.size() - 1)
            file << ",";
    }
    file << "]";
};

carteIdentite CoucheGenealogique::get(int index) {
    return ids[index];
};

void CoucheGenealogique::write(std::ofstream &file) {
    WRITE(expectedSize);
    // pour être certains du type
    unsigned int trueSize = ids.size();
    WRITE(trueSize);

    for (auto id : ids) {
        WRITE(id);
    }
};

void CoucheGenealogique::read(std::ifstream &file, CoucheGenealogique *dest) {
    unsigned int expectedSize;
    READ(expectedSize);
    dest->resize(expectedSize);

    unsigned int trueSize;
    READ(trueSize);

    for (int i = 0; i < trueSize; i++) {
        carteIdentite id;
        READ(id);
        dest->pushId(id.id, id.p1, id.p2);
    }
};

ArbreGenealogique::ArbreGenealogique(unsigned int couchesSize) {
    this->couchesSize = couchesSize;
}

unsigned int ArbreGenealogique::size() {
    return couches.size();
};

// ajoute une identité à la derniere couche
void ArbreGenealogique::pushId(cid_t targetId, cid_t p1, cid_t p2) {
    couches.back().pushId(targetId, p1, p2);
};
void ArbreGenealogique::ajouteCouche() {
    couches.push_back(CoucheGenealogique(couchesSize));
};

void ArbreGenealogique::writeJson(std::ofstream &file) {
    file << "[";
    int virgules = couches.size() - 1;
    for (auto &couche : couches) {
        couche.writeJson(file);
        if (virgules > 0) {
            file << ",";
			virgules--;
        }
    }
    file << "]";
};
void ArbreGenealogique::write(std::ofstream &file) {
    WRITE(couchesSize);
    auto csize = size();
    WRITE(csize);
    for (auto &couche : couches) {
        couche.write(file);
    }
};
void ArbreGenealogique::read(std::ifstream &file, ArbreGenealogique *dest) {
    READ(dest->couchesSize);
    unsigned int cSize;
    READ(cSize);
    for (int i = 0; i < cSize; i++) {
        CoucheGenealogique c(0);
        CoucheGenealogique::read(file, &c);
        dest->couches.push_back(c);
    }
};
