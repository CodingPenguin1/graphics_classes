#include "vector.h"
#include <math.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>


Vector::Vector() {
    this->size = 3;
    this->values = (float *)malloc(this->size * sizeof(float));
    for (int i = 0; i < this->size; ++i)
        this->values[i] = 0.0;
}


Vector::Vector(int size) {
    this->size = size;
    this->values = (float *)malloc(this->size * sizeof(float));
    for (int i = 0; i < this->size; ++i)
        this->values[i] = 0.0;
}


Vector::Vector(float a, float b) {
    this->size = 2;
    this->values = (float *)malloc(this->size * sizeof(float));
    this->values[0] = a;
    this->values[1] = b;
}


Vector::Vector(float a, float b, float c) {
    this->size = 3;
    this->values = (float *)malloc(this->size * sizeof(float));
    this->values[0] = a;
    this->values[1] = b;
    this->values[2] = c;
}


Vector::Vector(float a, float b, float c, float d) {
    this->size = 4;
    this->values = (float *)malloc(this->size * sizeof(float));
    this->values[0] = a;
    this->values[1] = b;
    this->values[3] = c;
    this->values[4] = d;
}


Vector::Vector(int size, float *values) {
    this->size = size;
    this->values = (float *)malloc(this->size * sizeof(float));
    for (int i = 0; i < this->size; ++i)
        this->values[i] = values[i];
}


Vector::Vector(Vector *vec) {
    this->size = vec->size;
    this->values = (float *)malloc(this->size * sizeof(float));
    for (int i = 0; i < this->size; ++i)
        this->values[i] = vec->values[i];
}


Vector::Vector(const Vector &vec) {
    this->size = vec.size;
    this->values = (float *)malloc(this->size * sizeof(float));
    for (int i = 0; i < this->size; ++i)
        this->values[i] = vec.values[i];
}


Vector::~Vector() {
    free(this->values);
}


Vector &Vector::operator=(const Vector &vec) {
    this->size = vec.size;
    this->values = (float *)malloc(this->size * sizeof(float));
    for (int i = 0; i < this->size; ++i)
        this->values[i] = vec.values[i];
    return *this;
}


float &Vector::operator[](const int i) {
    return this->values[i];
}


Vector Vector::operator+(const Vector &vec) {
    if (this->size == vec.size) {
        Vector sum = Vector(this->size);
        for (int i = 0; i < this->size; ++i)
            sum.values[i] = this->values[i] + vec.values[i];
        return sum;
    } else {
        throw std::length_error("Vector of size " + std::to_string(vec.size) + " cannot be added to vector of size " + std::to_string(this->size));
    }
}


Vector Vector::operator+(const float a) {
    Vector sum = Vector(this->size);
    for (int i = 0; i < this->size; ++i)
        sum.values[i] = this->values[i] + a;
    return sum;
}


Vector Vector::operator-(const Vector &vec) {
    if (this->size == vec.size) {
        Vector sum = Vector(this->size);
        for (int i = 0; i < this->size; ++i)
            sum.values[i] = this->values[i] - vec.values[i];
        return sum;
    } else {
        throw std::length_error("Vector of size " + std::to_string(vec.size) + " cannot be subtracted from vector of size " + std::to_string(this->size));
    }
}


Vector Vector::operator-(const float a) {
    Vector sum = Vector(this->size);
    for (int i = 0; i < this->size; ++i)
        sum.values[i] = this->values[i] - a;
    return sum;
}


Vector Vector::operator*(const Vector &vec) {
    if (this->size == vec.size) {
        Vector sum = Vector(this->size);
        for (int i = 0; i < this->size; ++i)
            sum.values[i] = this->values[i] * vec.values[i];
        return sum;
    } else {
        throw std::length_error("Vector of size " + std::to_string(this->size) + " cannot be multiplied by vector of size " + std::to_string(vec.size));
    }
}


Vector Vector::operator*(const float a) {
    Vector sum = Vector(this->size);
    for (int i = 0; i < this->size; ++i)
        sum.values[i] = this->values[i] * a;
    return sum;
}


Vector Vector::operator/(const Vector &vec) {
    if (this->size == vec.size) {
        Vector sum = Vector(this->size);
        for (int i = 0; i < this->size; ++i)
            sum.values[i] = this->values[i] / vec.values[i];
        return sum;
    } else {
        throw std::length_error("Vector of size " + std::to_string(this->size) + " cannot be divided by vector of size " + std::to_string(vec.size));
    }
}


Vector Vector::operator/(const float a) {
    if (a != 0.0) {
        Vector sum = Vector(this->size);
        for (int i = 0; i < this->size; ++i)
            sum.values[i] = this->values[i] / a;
        return sum;
    } else {
        throw std::logic_error("Vector cannot be divided by 0");
    }
}


float Vector::magnitude() {
    float magnitude = 0.0;
    for (int i = 0; i < this->size; ++i)
        magnitude += powf(this->values[i], 2.0);
    return sqrtf(magnitude);
}


float Vector::dot(const Vector &vec) {
    if (this->size == vec.size) {
        float product = 0.0;
        for (int i = 0; i < this->size; ++i)
            product += this->values[i] * vec.values[i];
        return product;
    } else {
        throw std::length_error("Dot product between vector of size " + std::to_string(vec.size) + " and vector of size " + std::to_string(this->size) + " is impossible");
    }
}


float Vector::distance(const Vector &vec) {
    if (this->size == vec.size) {
        float distance = 0.0;
        for (int i = 0; i < this->size; ++i)
            distance += pow(this->values[i] - vec.values[i], 2);
        return sqrt(distance);
    } else {
        throw std::length_error("Distance between vector of size " + std::to_string(vec.size) + " and vector of size " + std::to_string(this->size) + " is impossible");
    }
}


float Vector::distance(const Vector *vec) {
    if (this->size == vec->size) {
        float distance = 0.0;
        for (int i = 0; i < this->size; ++i)
            distance += pow(this->values[i] - vec->values[i], 2);
        return sqrt(distance);
    } else {
        throw std::length_error("Distance between vector of size " + std::to_string(vec->size) + " and vector of size " + std::to_string(this->size) + " is impossible");
    }
}


Vector Vector::cross(const Vector &vec) {
    if (this->size == 3 && vec.size == 3) {
        Vector product = Vector(3);
        product.values[0] = this->values[1] * vec.values[2] + this->values[2] * vec.values[1];
        product.values[1] = this->values[2] * vec.values[0] + this->values[0] * vec.values[2];
        product.values[2] = this->values[0] * vec.values[1] + this->values[1] * vec.values[0];
        return product;
    } else {
        throw std::length_error("Cross product between vector of size " + std::to_string(vec.size) + " and vector of size " + std::to_string(this->size) + " is impossible");
    }
}


Vector Vector::normalize() {
    Vector normalized = Vector(this->size);
    for (int i = 0; i < this->size; ++i)
        normalized[i] = this->values[i] / this->magnitude();
    return normalized;
}


bool Vector::operator==(const Vector &vec) {
    if (this->size == vec.size) {
        for (int i = 0; i < vec.size; ++i)
            if (this->values[i] != vec.values[i])
                return false;
        return true;
    } else {
        return false;
    }
}
