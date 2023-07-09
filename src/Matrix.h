#pragma once
#include <fstream>

class Matrix {
  public:
    int col = 0, ligne = 0;

  private:
    double *t;

  public:
    Matrix(){};
    Matrix(int n, int p);

    ~Matrix();

    inline double get(int i, int j) {
        if (i >= this->ligne || j >= this->col) {
            throw std::invalid_argument("Bad matrice get");
        }

        return this->t[i * this->col + j];
    }

    inline void set(int i, int j, double x) {
        if (i >= this->ligne || j >= this->col) {
            throw std::invalid_argument("Bad matrice set");
        }

        this->t[i * this->col + j] = x;
    }

    // Multiple deux matrices en place dans this (a * this)
    inline void mult_inv(Matrix &a) {
        if (this->ligne != a.col) {
            throw std::invalid_argument("lol multinv");
        }

        double *nt = new double[a.ligne * this->col];

        for (int i = 0; i < a.ligne; i++) {
            for (int j = 0; j < this->col; j++) {
                nt[i * this->col + j] = 0;
                for (int k = 0; k < this->ligne; k++) {
                    nt[i * this->col + j] += a.get(i, k) * this->get(k, j);
                }
            }
        }

        delete[] this->t;
        this->t = nt;
        this->ligne = a.ligne;
    }

    void print();

    Matrix *crossover(Matrix *a);
    void initialize();

    void write(std::ofstream &file);
    static Matrix *read(std::ifstream &file);
};

Matrix *mutation(Matrix &m);
