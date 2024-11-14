#ifndef __VECTOR_H__
#define __VECTOR_H__

class Vector {
  public:
    float *values;
    float size;
    Vector();
    Vector(int);
    Vector(float, float);
    Vector(float, float, float);
    Vector(float, float, float, float);
    Vector(int, float *);
    ~Vector();
    float operator[](const int);
    Vector operator+(const Vector &);
    Vector operator+(const int);
    Vector operator-(const Vector &);
    Vector operator-(const int);
    Vector operator*(const Vector &);
    Vector operator*(const int);
    Vector operator/(const Vector &);
    Vector operator/(const int);
    float magnitude();
    float dot(const Vector &);
    Vector cross(const Vector &);
    bool operator==(const Vector &);
};

#endif