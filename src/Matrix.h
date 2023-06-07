#pragma once
class Matrix {
  public:
    int col = 0, ligne = 0;

  private:
    double **t;

    void initialize();

  public:
    Matrix(){};
    Matrix(int n, int p);

    ~Matrix();

    double get(int i, int j);

    void set(int i, int j, double x);

    double trace();

    void print();

    void transpose();

    void mult(Matrix *a);
    void mult_inv(Matrix *a);

    void add(Matrix *a);

    void mult_scal(double x);
};
