#include <cmath>

double a_tanh(double x) {
    return tanh(x);
}

double ReLu(double x) {
    return fmax(0, x);
}

double sigmoide(double x) {
    return 1 / (1 + exp(-x));
}

double Heaviside(double x) {
    return (x < 0 ? 0 : 1);
}
