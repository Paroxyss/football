#include "util.hpp"
#include <ostream>

std::ostream &operator<<(std::ostream &out, gameStatistics stats) {
    out << "Stats{ collisions : "
        << "matches : " << stats.n << ", collisions : " << stats.totalCollisions
        << " | " << stats.totalCollisions / (double)(stats.n)
        << ", goals : " << stats.totalGoals << " | "
        << stats.totalGoals / (double)(stats.n) << " }";
    return out;
}

std::ostream &operator<<(std::ostream &out, gameInformations stats) {
    out << "Stats{ collisions : " << stats.collisions
        << ", goals : " << stats.goals << ", score : " << stats.score << " }";
    return out;
}
