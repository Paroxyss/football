#include <iomanip>

#include "util.hpp"
#include "Rand.hpp"

int previous_power(int n) {
    while (n & n - 1) {
        n = n & n - 1;
    }

    return n;
}

double angle_rounded(double input) {
	input = fmod(input + M_PI, 2 * M_PI);
	if (input < 0) {
		input += 2 * M_PI;
	}

	return input - M_PI;
}

#define DIVN(name) stats.name / (double)stats.n
#define DIVLN(name) stats.name / std::log2((double)stats.n)
std::ostream &operator<<(std::ostream &out, gameStatistics stats) {
	out << std::setprecision(3) << std::setfill(' ') << "S(" << stats.n
		<< "){c: " << std::setw(8) << DIVN(totalCollisions) << " | "
		<< std::setw(8) << DIVN(totalBallCollisions)
		<< "; g: " << std::setw(8) << DIVN(totalGoals)
		<< "; s: " << std::setw(8)
		<< (DIVN(scoreBleu) + DIVN(scoreRouge)) / 2 << "}";
	return out;
}

std::ostream &operator<<(std::ostream &out, gameInformations stats) {
	out << "Stats{ collisions : " << stats.collisions
		<< ", goals : " << stats.goals << ", score : " << stats.scoreBleu
		<< " | " << stats.scoreRouge << " }";
	return out;
}
