#pragma once
class Matrix {
  public:
    int col = 0, ligne = 0;

  private:
    double **t;

  public:
    Matrix(){};
    Matrix(int n, int p);

    ~Matrix();

    Matrix &operator=(Matrix &m);
    double get(int i, int j);

    void set(int i, int j, double x);

    void print();

    void mult(Matrix *a);
    void mult_inv(Matrix *a);

    Matrix *crossover(Matrix *a);
    void randomize();
};

Matrix *mutation(Matrix *m);
