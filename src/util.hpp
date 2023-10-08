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
    int stuck_reset;
};

struct gameStatistics {
    unsigned int n;
    unsigned long totalCollisions;
    unsigned int totalGoals;
    int total_ball_collisions;
    int stopped;
    int stuck_reset;
};

double randomDouble(double min, double max);
int thrand(int min, int max);
double randomDouble();
int previous_power(int n);
int random_power(int n);
double angleRounded(double input);
bool likelyness(double v);

std::ostream &operator<<(std::ostream &out, gameStatistics stats);
std::ostream &operator<<(std::ostream &out, gameInformations stats);
