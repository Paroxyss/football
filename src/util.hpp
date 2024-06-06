#pragma once

#include <iostream>
#include <ostream>

struct gameInformations {
    unsigned int collisions;
    unsigned int goals;
    int ballCollisions;
    double scoreRouge;
    double scoreBleu;
    bool stopped;
    double touchMean;
    double bonusRouge;
    double bonusBleu;
};

struct gameStatistics {
    unsigned int n;
    unsigned long totalCollisions;
    unsigned int totalGoals;
    int totalBallCollisions;
    int stopped;
    double scoreRouge;
    double scoreBleu;
};

double angle_rounded(double input);

// Retourne la puissance de 2 précédente
int previous_power(int n);

std::ostream &operator<<(std::ostream &out, gameStatistics stats);
std::ostream &operator<<(std::ostream &out, gameInformations stats);

#define WRITE_EXPL(vname, file) file.write((char *)&vname, sizeof(vname))
#define WRITE(vname) WRITE_EXPL(vname, file)

#define READ_EXPL(vname, file) file.read((char *)&vname, sizeof(vname))
#define READ(vname) READ_EXPL(vname, file)

#define FILE_METHOD(method, constructor)                                   \
    void method(std::constructor &file);                                   \
    void method(std::string filename) {                                    \
        std::constructor file;                                             \
        file.open(filename);                                               \
        if (!file.is_open())                                               \
            throw std::invalid_argument("File not found");                 \
        method(file);                                                      \
        file.close();                                                      \
    };

#define INFILE_METHOD(method) FILE_METHOD(method, ifstream)
#define OUTFILE_METHOD(method) FILE_METHOD(method, ofstream)
