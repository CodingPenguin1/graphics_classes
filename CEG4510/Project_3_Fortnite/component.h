#include "ply.h"
#include "vector.h"

class Component {
  private:
    void read_ply(char *);

  public:
    Vertex **vertex_list;
    Face **face_list;
    int num_vertices;
    int num_faces;

    Vector *parent_joint_vertex;
    Vector *parent_joint_angle;
    Vector *original_parent_joint_vertex;
    int num_children;
    Component **children;
    Component *parent;
    float length;

    Component(char *);
    Component(char *, float, float, float);
    ~Component();
    bool add_child(Component *child);
};