#include "Network.hpp"
#include "config.h"
#include "util.hpp"
#include <array>
#include <cstdio>
#include <iostream>
#include <ostream>
#include "Matrix.h"

int main() {
	auto n = Network({4,3,5});
	n.print();
	n.evaluate({1,2,3,4});
	n.evaluate({1,2,3,4});
	n.evaluate({1,2,3,4});
	n.evaluate({1,2,3,4});
	n.evaluate({1,2,3,4});
	n.mutate(0.5, 1);
	n.mutate(0.5, 1);
	n.mutate(0.5, 1);
	n.print();
}
