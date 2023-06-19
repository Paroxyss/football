#pragma once
#include <fstream>

class Matrix {
  public:
    int col = 0, ligne = 0;

  private:
    double **t;

  public:
    Matrix(){};
    Matrix(int n, int p);

    ~Matrix();

    inline double get(int i, int j) {
        /*if (i >= this->ligne || j >= this->col) {
            throw std::invalid_argument("Bad matrice get");
        }*/

        return this->t[i][j];
    }

    inline void set(int i, int j, double x) {
        /*if (i >= this->ligne || j >= this->col) {
            throw std::invalid_argument("Bad matrice set");
        }*/

        this->t[i][j] = x;
    }

    // Multiple deux matrices en place dans this (a * this)
    inline void mult_inv(Matrix *a) {
        if (this->ligne != a->col) {
            throw std::invalid_argument("lol multinv");
        }

        double **newT = new double *[a->ligne];
        for (int i = 0; i < a->ligne; i++) {
            double *v = (double *)calloc(this->col, sizeof(double));
            for (int j = 0; j < this->col; j++) {
                for (int k = 0; k < this->ligne; k++) {
                    v[j] += a->get(i, k) * this->get(k, j);
                }
            }
            newT[i] = v;
        }

        for (int i = 0; i < this->ligne; i++) {
            delete this->t[i];
        }

        delete[] t;
        this->t = newT;
        this->ligne = a->ligne;
    }

    void print();

    Matrix *crossover(Matrix *a);
    void randomize();

    void write(std::ofstream file);
};

Matrix *mutation(Matrix *m);
