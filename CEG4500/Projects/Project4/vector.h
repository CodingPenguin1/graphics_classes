#include <iostream>
#include <math.h>
#include <stdio.h>


class Vector {
  private:
  public:
    float x, y, z;
    Vector(float x, float y, float z);
    float dot(Vector vector);
    float operator*(Vector vector);
    Vector cross(Vector vector);
    Vector add(Vector vector);
    Vector operator+(Vector vector);
    Vector subtract(Vector vector);
    Vector operator-(Vector vector);
    Vector scalar_multiply(float scalar);
    Vector operator*(float scalar);
    Vector operator/(float scalar);
    float magnitude();
    Vector normalize();
};

std::ostream &operator<<(std::ostream &stream, Vector &vector);
