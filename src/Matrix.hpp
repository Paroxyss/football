#pragma once

#include <fstream>

#define MATRIX_SIZE 30 * 30

class Matrix {
  private:
    // Tableau de données
    double *t;
    // Tableau de calculs
    double *ct;

    // Une telle architecture est très couteuse en espace, mais reste minime
    // par rapport à la taille de la RAM Le bénéfice temporel est très
    // grands: (x1.5-x2)

  public:
    int col = 0;
    int ligne = 0;

    Matrix(){};
    Matrix(int n, int p);
    ~Matrix();

    // On note this la matrice contenue par l'instance

    //  this_ij
    double get(int i, int j);
    // this_ij <- x
    void set(int i, int j, double x);

    // this <- a*this
    void mult_inv(Matrix &a);

    void print();

    // Initialise une matrice avec des coefficients aléatoires
    void initialize();

    // Copie les coefficients de source dans destination
    static void clone(Matrix *source, Matrix *dest);

    // Sauvegarde la matrice dans un fichier binaire
    void write(std::ofstream &file);
    // Chargement de la matrice depuis un fichier binaire
    static Matrix *read(std::ifstream &file);
};
