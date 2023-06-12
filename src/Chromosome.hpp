#include "Matrix.h"
#include "config.h"

class Chromosome {
  public:
    Matrix *matrix[EQUIPE_SIZE][NETWORK_SIZE - 1];

    void initialize();
    ~Chromosome();

    void print();

    void randomize();
};

Matrix *evaluate(Matrix *inputs, Chromosome *c);
