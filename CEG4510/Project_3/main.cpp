#include "component.h"
#include "vector.h"
#include <GL/glut.h>
#include <chrono>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>


#define DISPLAY_SIZE 1000
#define PI 3.14159265
#define GRID_SIZE 4


Component skeleton = Component("skeleton/torso_upper_full.ply");
Vector **right_leg_targets = new Vector *[4];
Vector **left_leg_targets = new Vector *[4];
int right_leg_target_index = 1;
int left_leg_target_index = 3;
float proximity_threshold = 5;


float map(float val, float old_start, float old_end, float new_start, float new_end) {
    return (val - old_start) / (old_end - old_start) * (new_end - new_start) + new_start;
}


double time() {
    return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();
}


float distance(Vector *a, Vector *b) {
    float sum = 0.0;
    for (int i = 0; i < a->size; ++i) {
        sum += pow(a->values[i] - b->values[i], 2);
    }
    return sqrt(sum);
}


float rad(float deg) {
    return deg * PI / 180.0;
}

float deg(float rad) {
    return rad * 180.0 / PI;
}


void draw_string(float x, float y, std::string s) {
    glRasterPos2f(x, y);
    for (int i = 0; i < (int)s.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
}


void draw_face(Component *c, int face_index) {
    glBegin(GL_TRIANGLES);
    Face *face = c->face_list[face_index];
    Vertex **vlist = c->vertex_list;
    for (int i = 0; i < face->nverts - 1; ++i) {
        glVertex3f(vlist[face->verts[0]]->x, vlist[face->verts[0]]->y, vlist[face->verts[0]]->z);
        glVertex3f(vlist[face->verts[i]]->x, vlist[face->verts[i]]->y, vlist[face->verts[i]]->z);
        glVertex3f(vlist[face->verts[i + 1]]->x, vlist[face->verts[i + 1]]->y, vlist[face->verts[i + 1]]->z);
    }
    glEnd();
}


void draw_component(Component *component) {
    // Draw all but last line
    glBegin(GL_LINES);
    for (int face_index = 0; face_index < component->num_faces; ++face_index) {
        draw_face(component, face_index);
    }
    glEnd();
}


void draw_components(Component *component) {
    draw_component(component);
    for (int i = 0; i < component->num_children; ++i)
        if (component->children[i] != nullptr)
            draw_components(component->children[i]);
}


void rotate_matrix(Component *component, float angle, int x, int y, int z) {
    float *parent_joint = component->original_parent_joint_vertex->values;
    glTranslatef(parent_joint[0], parent_joint[1], parent_joint[2]);
    glRotatef(angle, x, y, z);
    glTranslatef(-parent_joint[0], -parent_joint[1], -parent_joint[2]);
}


void draw_leg(Component *upper_leg, float hip_angle, float knee_angle, float ankle_angle) {
    Component *lower_leg = upper_leg->children[0];
    Component *foot = lower_leg->children[0];

    // Hip rotation
    glPushMatrix();
    rotate_matrix(upper_leg, hip_angle, 1, 0, 0);
    upper_leg->parent_joint_angle = new Vector(hip_angle, 0, 0);
    draw_component(upper_leg);

    // Knee rotation
    glPushMatrix();
    rotate_matrix(lower_leg, knee_angle, 1, 0, 0);
    lower_leg->parent_joint_angle = new Vector(knee_angle, 0, 0);
    float x = upper_leg->parent_joint_vertex->values[0];
    float y = upper_leg->length * sin(rad(hip_angle)) + upper_leg->parent_joint_vertex->values[1];
    float z = -upper_leg->length * cos(rad(hip_angle)) + upper_leg->parent_joint_vertex->values[2];
    lower_leg->parent_joint_vertex = new Vector(x, y, z);
    draw_component(lower_leg);

    // Ankle rotation
    glPushMatrix();
    rotate_matrix(foot, ankle_angle, 1, 0, 0);
    foot->parent_joint_angle = new Vector(ankle_angle, 0, 0);
    x = lower_leg->parent_joint_vertex->values[0];
    y = lower_leg->length * sin(rad(180 - (hip_angle + knee_angle))) + lower_leg->parent_joint_vertex->values[1];
    z = lower_leg->length * cos(rad(180 - (hip_angle + knee_angle))) + lower_leg->parent_joint_vertex->values[2];
    foot->parent_joint_vertex = new Vector(x, y, z);
    printf("Ankle: %f %f\n", y, z);
    draw_component(foot);

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}


int target(Component *leg, Vector **target_list, int target_index) {
    Component *lower_leg = leg->children[0];
    Component *foot = lower_leg->children[0];

    // ==========================
    // = Upper Leg Calculations =
    // ==========================
    // This math is wild, I'm not even going to try to explain it
    // See the pdfs in this directory to see how it works
    float y0 = leg->parent_joint_vertex->values[1];
    float z0 = leg->parent_joint_vertex->values[2];
    float y1 = lower_leg->parent_joint_vertex->values[1];
    float z1 = lower_leg->parent_joint_vertex->values[2];
    float y2 = foot->parent_joint_vertex->values[1];
    float z2 = foot->parent_joint_vertex->values[2];
    float theta0 = leg->parent_joint_angle->values[0];
    float theta1 = lower_leg->parent_joint_angle->values[0];
    float a = target_list[target_index]->values[1];
    float b = target_list[target_index]->values[2];
    Vector *target = new Vector(a, b);
    Vector vec_a = Vector(y1 - y0, z1 - z0);
    Vector vec_b = Vector(y2 - y1, z2 - z1);
    Vector vec_c = Vector(a - y0, b - z0);
    Vector vec_d = Vector(a - y1, b - z1);

    float phi = acos((vec_a.dot(vec_c)) / (vec_a.magnitude() * vec_c.magnitude()));
    float alpha = phi / 50.0;

    float y2_prime_pos = (y2 - y0) * cos(alpha) - (z2 - z0) * sin(alpha) + y0;
    float z2_prime_pos = (z2 - z0) * cos(alpha) + (y2 - y0) * sin(alpha) + z0;
    Vector *ankle_upper_move_pos = new Vector(y2_prime_pos, z2_prime_pos);
    float d_upper_pos = distance(ankle_upper_move_pos, target);

    float y2_prime_neg = (y2 - y0) * cos(-alpha) - (z2 - z0) * sin(-alpha) + y0;
    float z2_prime_neg = (z2 - z0) * cos(-alpha) + (y2 - y0) * sin(-alpha) + z0;
    Vector *ankle_upper_move_neg = new Vector(y2_prime_neg, z2_prime_neg);
    float d_upper_neg = distance(ankle_upper_move_neg, target);

    float d_upper;
    float theta0_prime;
    if (abs(d_upper_pos) < abs(d_upper_neg)) {
        theta0_prime = deg(alpha) + theta0;
        d_upper = abs(d_upper_pos);
    } else {
        d_upper = abs(d_upper_neg);
        theta0_prime = deg(-alpha) + theta0;
    }

    // ==========================
    // = Lower Leg Calculations =
    // ==========================
    phi = acos((vec_b.dot(vec_d)) / (vec_b.magnitude() * vec_d.magnitude()));
    alpha = phi / 50.0;

    y2_prime_pos = (y2 - y1) * cos(alpha) - (z2 - z1) * sin(alpha) + y1;
    z2_prime_pos = (z2 - z1) * cos(alpha) + (y2 - y1) * sin(alpha) + z1;
    ankle_upper_move_pos = new Vector(y2_prime_pos, z2_prime_pos);
    float d_lower_pos = distance(ankle_upper_move_pos, target);

    y2_prime_neg = (y2 - y1) * cos(-alpha) - (z2 - z1) * sin(-alpha) + y1;
    z2_prime_neg = (z2 - z1) * cos(-alpha) + (y2 - y1) * sin(-alpha) + z1;
    ankle_upper_move_neg = new Vector(y2_prime_neg, z2_prime_neg);
    float d_lower_neg = distance(ankle_upper_move_neg, target);

    float d_lower;
    float theta1_prime;
    if (abs(d_lower_pos) < abs(d_lower_neg)) {
        theta1_prime = (deg(alpha) + theta1);
        d_lower = abs(d_lower_pos);
    } else {
        d_lower = abs(d_lower_neg);
        theta1_prime = (deg(-alpha) + theta1);
    }

    printf("%f %f %f %f\n", d_upper_pos, d_upper_neg, d_lower_pos, d_lower_neg);

    // ================
    // = Leg Movement =
    // ================
    if (d_upper < d_lower) {
        // Check for and set new target
        if (d_upper < proximity_threshold) {
            target_index++;
            if (target_index >= 4) target_index = 0;
        }
        printf("Distance: %f\n", d_upper);
        printf("Target: %d\n", target_index);
        printf("Upper: %f (at %f)\n", theta0_prime, theta0);
        draw_leg(leg, theta0_prime, theta1, 0);
    } else {
        // Check for and set new target
        if (d_lower < proximity_threshold) {
            target_index++;
            if (target_index >= 4) target_index = 0;
        }
        printf("Distance: %f\n", d_lower);
        printf("Target: %d\n", target_index);
        printf("Lower: %f (at %f)\n", theta1_prime, theta1);
        draw_leg(leg, theta0, theta1_prime, 0);
    }

    return target_index;
}


void display() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_LIGHTING);

    draw_component(&skeleton);

    Component *right_leg = skeleton.children[0];
    Component *left_leg = skeleton.children[1];

    right_leg_target_index = target(right_leg, right_leg_targets, right_leg_target_index);
    left_leg_target_index = target(left_leg, left_leg_targets, left_leg_target_index);

    glFlush();
    glutSwapBuffers();
}


int main(int argc, char **argv) {
    // Set up window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(DISPLAY_SIZE, DISPLAY_SIZE);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("[floating] CEG 4510 Project 3");

    // Callbacks
    glutDisplayFunc(display);
    glutIdleFunc(glutPostRedisplay);

    // Set up rendering and lighting
    glEnable(GL_NORMALIZE);
    // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glMatrixMode(GL_PROJECTION);
    glOrtho(-50, 50, -50, 50, 100000, -100000);

    // GLfloat lightLocation[4] = {-20, 20, 100, 3};
    GLfloat lightLocation[4] = {-40, 40, 200, 7};
    GLfloat lightGray[4] = {0.8, 0.8, 0.8, 1};
    GLfloat darkGray[4] = {0.2, 0.2, 0.2, 1};
    GLfloat black[4] = {0, 0, 0, 1};

    glLightfv(GL_LIGHT0, GL_POSITION, lightLocation);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightGray);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightGray);
    glLightfv(GL_LIGHT0, GL_AMBIENT, darkGray);

    glMaterialfv(GL_FRONT, GL_AMBIENT, darkGray);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lightGray);
    glMaterialfv(GL_FRONT, GL_SPECULAR, lightGray);
    glMaterialfv(GL_FRONT, GL_EMISSION, black);
    glMaterialf(GL_FRONT, GL_SHININESS, 100);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Load skeleton and arrage parts
    Component *right_leg_upper = new Component("skeleton/right_leg_upper.ply", -2.4404, 0.3677, -7.3972);
    skeleton.add_child(right_leg_upper);
    Component *right_leg_lower = new Component("skeleton/right_leg_lower.ply", -3.2124, 1.8347, -24.248);
    right_leg_upper->length = distance(right_leg_upper->parent_joint_vertex, right_leg_lower->parent_joint_vertex);
    right_leg_upper->add_child(right_leg_lower);
    Component *right_foot = new Component("skeleton/right_foot.ply", -3.0856, 3.1448, -39.508);
    right_leg_lower->length = distance(right_leg_lower->parent_joint_vertex, right_foot->parent_joint_vertex);
    right_leg_lower->add_child(right_foot);

    Component *left_leg_upper = new Component("skeleton/left_leg_upper.ply", 2.4404, 0.3677, -7.3972);
    skeleton.add_child(left_leg_upper);
    Component *left_leg_lower = new Component("skeleton/left_leg_lower.ply", 3.2124, 1.8347, -24.248);
    left_leg_upper->length = distance(left_leg_upper->parent_joint_vertex, left_leg_lower->parent_joint_vertex);
    left_leg_upper->add_child(left_leg_lower);
    Component *left_foot = new Component("skeleton/left_foot.ply", 3.0856, 3.1448, -39.508);
    left_leg_lower->length = distance(left_leg_lower->parent_joint_vertex, left_foot->parent_joint_vertex);
    left_leg_lower->add_child(left_foot);

    // Target points for CCD
    right_leg_targets[0] = new Vector(-3.0856, 3.1448, -39.508);
    right_leg_targets[1] = new Vector(-3.0856, 10, -30);
    right_leg_targets[2] = new Vector(-3.0856, 3.1448, -38);
    right_leg_targets[3] = new Vector(-3.0856, -15, -30);
    left_leg_targets[0] = new Vector(-3.0856, 3.1448, -39.508);
    left_leg_targets[1] = new Vector(-3.0856, 10, -30);
    left_leg_targets[2] = new Vector(-3.0856, 3.1448, -38);
    left_leg_targets[3] = new Vector(-3.0856, -15, -30);

    gluLookAt(-300, 500, -400, 0, 0, 0, 0, 0, 1);
    glutMainLoop();

    // Exit
    delete[] right_leg_targets;
    delete[] left_leg_targets;
    return 0;
}
