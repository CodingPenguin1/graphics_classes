#ifndef __VECTOR_H__
#define __VECTOR_H__

class Vector {
  public:
    float *values;
    int size;
    Vector();
    Vector(int);
    Vector(float, float);
    Vector(float, float, float);
    Vector(float, float, float, float);
    Vector(int, float *);
    Vector(Vector *);
    Vector(const Vector &);
    ~Vector();
    Vector &operator=(const Vector &vec);
    float &operator[](const int);
    Vector operator+(const Vector &);
    Vector operator+(const float);
    Vector operator-(const Vector &);
    Vector operator-(const float);
    Vector operator*(const Vector &);
    Vector operator*(const float);
    Vector operator/(const Vector &);
    Vector operator/(const float);
    float magnitude();
    float dot(const Vector &);
    float distance(const Vector &);
    float distance(const Vector *);
    Vector cross(const Vector &);
    Vector normalize();
    bool operator==(const Vector &);
};

#endif