#pragma once

#include <cmath>
#include <iostream>
#include <ostream>
#include <random>
#include <thread>

struct gameInformations {
    unsigned int collisions;
    unsigned int goals;
    int ball_collisions;
    double score;
    bool stopped;
};

struct gameStatistics {
    unsigned int n;
    unsigned long totalCollisions;
    unsigned int totalGoals;
    int total_ball_collisions;
    int stopped;
};

double randomDouble(double min, double max);
int thrand(int min, int max);
double randomDouble();
int previous_power(int n);
int random_power(int n);
double angleRounded(double input);
bool likelyness(double v);
double binomialRestreint(double p, double maxi);

std::ostream &operator<<(std::ostream &out, gameStatistics stats);
std::ostream &operator<<(std::ostream &out, gameInformations stats);

#define WRITE_EXPL(vname, file) file.write((char *)&vname, sizeof(vname))
#define WRITE(vname) WRITE_EXPL(vname, file)

#define READ_EXPL(vname, file) file.read((char *)&vname, sizeof(vname))
#define READ(vname) READ_EXPL(vname, file)

#define FILE_METHOD(method, constructor)                                       \
    void method(std::constructor &file);                                       \
    void method(std::string filename) {                                        \
        std::constructor file;                                                 \
        file.open(filename);                                                   \
        if (!file.is_open())                                                  \
            throw std::invalid_argument("File not found");                     \
        method(file);                                                          \
        file.close();                                                          \
    };

#define INFILE_METHOD(method) FILE_METHOD(method, ifstream)
#define OUTFILE_METHOD(method) FILE_METHOD(method, ofstream)
