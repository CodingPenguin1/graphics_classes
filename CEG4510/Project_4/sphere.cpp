#include "sphere.h"
#include <stdio.h>
#include <stdlib.h>


Sphere::Sphere(float radius) {
    this->radius = radius;
    this->mass = 1;
    this->color = new Vector(this->random(0.5, 1.0), this->random(0.5, 1.0), this->random(0.5, 1.0));
    this->velocity = new Vector(0.0, 0.0, 0.0);
    this->location = new Vector(0.0, 0.0, 0.0);
}


Sphere::Sphere(float radius, float x, float y, float z) {
    this->radius = radius;
    this->mass = 1;
    this->color = new Vector(this->random(0.5, 1.0), this->random(0.5, 1.0), this->random(0.5, 1.0));
    this->velocity = new Vector(0.0, 0.0, 0.0);
    this->location = new Vector(x, y, z);
}


Sphere::~Sphere() {
}


float Sphere::random(float min, float max) {
    return min + (float)(rand()) / ((float)(RAND_MAX / (max - min)));
}


bool Sphere::is_touching(Sphere *sphere) {
    return (this->location->distance(sphere->location) <= this->radius + sphere->radius);
}


void Sphere::collide_with(Sphere *sphere) {
    // Math and symbols taken from https://www.sjsu.edu/faculty/watkins/collision.htm
    float m1 = this->mass;
    float m2 = sphere->mass;
    Vector x1(this->location);
    Vector x2(sphere->location);
    Vector u1(this->velocity);
    Vector u2(sphere->velocity);
    Vector v1 = Vector(3);
    Vector v2 = Vector(3);

    Vector k = Vector(3);
    k = (x1 - x2) / (x1 - x2).magnitude();
    float a = 2 * k.dot((u1 - u2)) / ((1.0 / m1) + (1.0 / m2));
    v1 = (((k * a) / m1) - u1) * -1;
    v2 = (((k * a) / -m2) - u2) * -1;

    this->velocity = new Vector(v1[0], v1[1], v1[2]);
    sphere->velocity = new Vector(v2[0], v2[1], v2[2]);
}
