#include "util.hpp"
#include <ostream>

std::ostream &operator<<(std::ostream &out, gameStatistics stats) {
    out << "Stats{ collisions : "
        << "matches : " << stats.n << ", collisions : " << stats.totalCollisions
        << " | " << stats.collisionsMean << ", goals : " << stats.totalGoals
        << " | " << stats.goalsMean << ", score : " << stats.scoreMean << " }";
    return out;
}

std::ostream &operator<<(std::ostream &out, gameInformations stats) {
    out << "Stats{ collisions : " << stats.collisions
        << ", goals : " << stats.goals << ", score : " << stats.score << " }";
    return out;
}
