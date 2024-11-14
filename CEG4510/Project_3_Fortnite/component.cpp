#include "component.h"
#include "ply.h"
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>


Component::Component(char *filename) {
    read_ply(filename);

    this->parent_joint_vertex = new Vector(3);
    this->parent_joint_angle = new Vector(3);
    this->original_parent_joint_vertex = new Vector(3);
    this->num_children = 5;
    this->children = (Component **)malloc(sizeof(Component *) * this->num_children);
    for (int i = 0; i < this->num_children; ++i)
        this->children[i] = nullptr;
    this->parent = nullptr;
    this->length = 0;
}


Component::Component(char *filename, float x, float y, float z) {
    read_ply(filename);

    this->parent_joint_vertex = new Vector(x, y, z);
    this->parent_joint_angle = new Vector(3);
    this->original_parent_joint_vertex = new Vector(x, y, z);
    this->num_children = 5;
    this->children = (Component **)malloc(sizeof(Component *) * this->num_children);
    for (int i = 0; i < this->num_children; ++i)
        this->children[i] = nullptr;
    this->parent = nullptr;
    this->length = 0;
}


Component::~Component() {
    for (int i = 0; i < this->num_children; ++i)
        // if (this->children[i] != nullptr)
        this->children[i]->parent = nullptr;
    free(this->children);

    for (int i = 0; i < this->num_faces; ++i)
        free(this->face_list[i]);
    free(this->face_list);

    for (int i = 0; i < this->num_vertices; ++i)
        free(this->vertex_list[i]);
    free(this->vertex_list);
}


void Component::read_ply(char *filename) {
    // int Component::read_ply(char *filename, Vertex **vlist, Face **flist) {
    int i, j, k;
    PlyFile *ply;
    int nelems;
    char **elist;
    int file_type;
    float version;
    int nprops;
    PlyProperty **plist;
    char *elem_name;
    int num_comments;
    char **comments;
    int num_obj_info;
    char **obj_info;
    int num_elems;

    /* open a PLY file for reading */
    ply = ply_open_for_reading(filename, &nelems, &elist, &file_type, &version);

    /* go through each kind of element that we learned is in the file */
    /* and read them */
    for (i = 0; i < nelems; i++) {
        /* get the description of the first element */
        elem_name = elist[i];
        plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);

        /* if we're on vertex elements, read them in */
        if (equal_strings("vertex", elem_name)) {
            /* create a vertex list to hold all the vertices */
            this->vertex_list = (Vertex **)malloc(sizeof(Vertex *) * num_elems);
            this->num_vertices = num_elems;

            /* set up for getting vertex elements */
            ply_get_property(ply, elem_name, &vert_props[0]);
            ply_get_property(ply, elem_name, &vert_props[1]);
            ply_get_property(ply, elem_name, &vert_props[2]);
            ply_get_property(ply, elem_name, &vert_props[3]);
            ply_get_property(ply, elem_name, &vert_props[4]);
            ply_get_property(ply, elem_name, &vert_props[5]);

            /* grab all the vertex elements */
            for (j = 0; j < num_elems; j++) {
                /* grab and element from the file */
                this->vertex_list[j] = (Vertex *)malloc(sizeof(Vertex));
                ply_get_element(ply, (void *)this->vertex_list[j]);
            }
        }

        /* if we're on face elements, read them in */
        if (equal_strings("face", elem_name)) {
            /* create a list to hold all the face elements */
            this->face_list = (Face **)malloc(sizeof(Face *) * num_elems);
            this->num_faces = num_elems;

            /* set up for getting face elements */
            ply_get_property(ply, elem_name, &face_props[0]);
            ply_get_property(ply, elem_name, &face_props[1]);

            /* grab all the face elements */
            for (j = 0; j < num_elems; j++) {
                /* grab and element from the file */
                this->face_list[j] = (Face *)malloc(sizeof(Face));
                ply_get_element(ply, (void *)this->face_list[j]);
            }
        }
    }

    /* grab and print out the comments in the file */
    comments = ply_get_comments(ply, &num_comments);

    /* grab and print out the object information */
    obj_info = ply_get_obj_info(ply, &num_obj_info);

    /* close the PLY file */
    ply_close(ply);
    // return num_elems;
}


bool Component::add_child(Component *child) {
    for (int i = 0; i < this->num_children; ++i) {
        if (this->children[i] == nullptr) {
            this->children[i] = child;
            child->parent = this;
            return true;
        }
    }
    return false;
}