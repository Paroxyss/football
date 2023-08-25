#pragma once
#include <fstream>

class Matrix {
  private:
    double *t;

  public:
    int col = 0, ligne = 0;
    Matrix(){};
    Matrix(int n, int p);
    ~Matrix();
    double get(int i, int j);
    void set(int i, int j, double x);
    void mult_inv(Matrix &a);
    void print();
    Matrix *crossover(Matrix *a);
    void initialize();
    void write(std::ofstream &file);
    static Matrix *read(std::ifstream &file);
};

void mutation(Matrix &m);
