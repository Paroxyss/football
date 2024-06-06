#pragma once

#include <cmath>
#include <ostream>

#include "util.hpp"

struct vector {
    double x, y;
};

inline vector copy(vector v) {
    return {.x = v.x, .y = v.y};
}

inline vector &operator+=(vector &a, vector b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline vector operator+(vector a, vector b) {
    vector result = copy(a);
    return result += b;
}

inline vector operator-(vector a) {
    return {.x = -a.x, .y = -a.y};
}

inline vector &operator-=(vector &a, vector b) {
    return a += -b;
}

inline vector operator-(vector a, vector b) {
    vector result = copy(a);
    return result -= b;
}

inline vector &operator*=(vector &a, double factor) {
    a.x *= factor;
    a.y *= factor;
    return a;
}

inline vector operator*(vector a, double factor) {
    vector result = copy(a);
    return result *= factor;
}

inline vector operator*(double factor, vector a) {
    return a * factor;
}

inline vector &operator/=(vector &a, double factor) {
    a.x /= factor;
    a.y /= factor;
    return a;
}

inline vector operator/(vector a, double factor) {
    vector result = copy(a);
    return result /= factor;
}

inline double dot_product(vector a, vector b) {
    return a.x * b.x + a.y * b.y;
}

inline double norme_carre(vector a) {
    return pow(a.x, 2) + pow(a.y, 2);
}

inline double norme(vector a) {
    return sqrt(norme_carre(a));
}

// deuxième fonction testée du programme
inline double vangle(vector v) {
    return angle_rounded(atan2(v.y, v.x));
}

inline std::ostream &operator<<(std::ostream &out, vector vec) {
    out << "Vector{ .x=" << vec.x << ", .y = " << vec.y << "}";
    return out;
}
