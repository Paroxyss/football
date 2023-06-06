#include "boost/numeric/ublas/matrix_expression.hpp"
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <ostream>

void incr(int &val) { val++; }

int main() {
    using namespace boost::numeric::ublas;
    matrix<int> m(3, 3);
    for (unsigned i = 0; i < m.size1(); ++i)
        for (unsigned j = 0; j < m.size2(); ++j)
            m(i, j) = 3 * i + j;

    matrix<int> n(3, 3);
    int a = 0;
    for (unsigned i = 0; i < m.size1(); ++i) {
        for (unsigned j = 0; j < m.size2(); ++j) {
            n(i, j) = a;
            incr(a);
        }
}

    std::cout << m << std::endl;
    std::cout << n << std::endl;
    std::cout << prod(m, n) << std::endl;
}
