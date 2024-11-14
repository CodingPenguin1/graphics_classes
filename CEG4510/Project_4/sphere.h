#include "vector.h"

class Sphere {
  private:
    float random(float, float);

  public:
    float radius;
    float mass;
    Vector *location;
    Vector *velocity;
    Vector *color;

    Sphere(float);
    Sphere(float, float, float, float);
    ~Sphere();
    bool is_touching(Sphere *);
    void collide_with(Sphere *);
};