#pragma once

#include "boost/numeric/ublas/matrix_expression.hpp"
#include <array>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <ostream>

#define NETWORK_SIZE 5

using namespace boost::numeric::ublas;
class Network {
    std::array<matrix<int>, NETWORK_SIZE> weight = std::array<matrix<int>, NETWORK_SIZE>();
	
};
