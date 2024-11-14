#include "vector.h"


Vector::Vector(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}


float Vector::dot(Vector vector) {
    return this->x * vector.x + this->y * vector.y + this->z * vector.z;
}


float Vector::operator*(Vector vector) {
    return this->dot(vector);
}


Vector Vector::cross(Vector vector) {
    float new_x = this->y * vector.z - this->z * vector.y;
    float new_y = this->z * vector.x - this->x * vector.z;
    float new_z = this->x * vector.y - this->y * vector.x;
    return Vector(new_x, new_y, new_z);
}


Vector Vector::add(Vector vector) {
    return Vector(this->x + vector.x, this->y + vector.y, this->z + vector.z);
}


Vector Vector::operator+(Vector vector) {
    return Vector(this->x + vector.x, this->y + vector.y, this->z + vector.z);
}


Vector Vector::subtract(Vector vector) {
    return Vector(this->x - vector.x, this->y - vector.y, this->z - vector.z);
}


Vector Vector::operator-(Vector vector) {
    return Vector(this->x - vector.x, this->y - vector.y, this->z - vector.z);
}


Vector Vector::scalar_multiply(float scalar) {
    return Vector(scalar * this->x, scalar * this->y, scalar * this->z);
}


Vector Vector::operator*(float scalar) {
    return Vector(scalar * this->x, scalar * this->y, scalar * this->z);
}

Vector Vector::operator/(float scalar) {
    return Vector(this->x / scalar, this->y / scalar, this->z / scalar);
}


float Vector::magnitude() {
    return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
}


Vector Vector::normalize() {
    return Vector(this->x / this->magnitude(), this->y / this->magnitude(), this->z / this->magnitude());
}


std::ostream &operator<<(std::ostream &stream, Vector &vector) {
    return stream << "<" << vector.x << "," << vector.y << "," << vector.z << ">";
}
