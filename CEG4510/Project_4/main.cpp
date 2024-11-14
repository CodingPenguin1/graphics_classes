#include "sphere.h"
#include <GL/glut.h>
#include <chrono>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define DISPLAY_SIZE 1000
#define PI 3.14159265

struct color {
    float r;
    float g;
    float b;
};

// Render variables
const color WHITE = color{1.0, 1.0, 1.0};
const color GRAY = color{0.5, 0.5, 0.5};
const color YELLOW = color{1.0, 1.0, 0.0};

// Camera controls
float cam_trans_x = 0.0;
float cam_trans_y = 0.0;
int cam_rot_x = 0;
int cam_rot_y = 0;
int cam_rot_z = 0;
float zoom = 1.0;

// Toggles
bool enable_gravity = true;
bool enable_lighting = true;

// Misc constants
const GLubyte stipple_mask[] = {
        0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
        0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
        0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
        0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
        0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
        0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
        0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
        0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
        0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
        0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
        0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
        0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
        0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
        0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
        0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
        0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00};
const float container_radius = 0.5;
const float container_height = 1.5;
const float ball_radius = 0.05;
const int ball_count = 10;

// Misc variables
bool *key_states = new bool[256];
bool *special_key_states = new bool[256];
const int framerate_average_count = 10;
double *frame_times = new double[framerate_average_count];
Sphere **balls = (Sphere **)malloc(sizeof(Sphere *) * ball_count);
float gravity = 0.0001;


// =====================
// = UTILITY FUNCTIONS =
// =====================


float map(float val, float old_start, float old_end, float new_start, float new_end) {
    return (val - old_start) / (old_end - old_start) * (new_end - new_start) + new_start;
}


double time() {
    return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();
}


float rad(float deg) {
    return deg * PI / 180.0;
}


float deg(float rad) {
    return rad * 180.0 / PI;
}


// ======================
// = KEYPRESS FUNCTIONS =
// ======================


void buffered_key_operations() {
    // Translation
    cam_trans_x = (key_states['d']) ? cam_trans_x + 0.01 : cam_trans_x;
    cam_trans_x = (key_states['a']) ? cam_trans_x - 0.01 : cam_trans_x;
    cam_trans_y = (key_states['s']) ? cam_trans_y - 0.01 : cam_trans_y;
    cam_trans_y = (key_states['w']) ? cam_trans_y + 0.01 : cam_trans_y;

    // Rotation
    cam_rot_x = (key_states['o']) ? cam_rot_x + 2 : cam_rot_x;
    cam_rot_x = (key_states['l']) ? cam_rot_x - 2 : cam_rot_x;
    cam_rot_y = (key_states['-']) ? cam_rot_y - 2 : cam_rot_y;  // I have a weird keyboard layout
    cam_rot_y = (key_states[';']) ? cam_rot_y - 2 : cam_rot_y;
    cam_rot_y = (key_states['k']) ? cam_rot_y + 2 : cam_rot_y;
    cam_rot_z = (key_states['p']) ? cam_rot_z - 2 : cam_rot_z;
    cam_rot_z = (key_states['i']) ? cam_rot_z + 2 : cam_rot_z;
    if (abs(cam_rot_x) >= 360) cam_rot_x %= 360;
    if (abs(cam_rot_y) >= 360) cam_rot_y %= 360;
    if (abs(cam_rot_z) >= 360) cam_rot_z %= 360;

    // Zoom
    zoom = (key_states['e']) ? zoom + 0.01 : zoom;
    zoom = (key_states['q']) ? zoom - 0.01 : zoom;
}


void key_down(unsigned char key, int x, int y) {
    key_states[key] = true;

    // Toggles
    if (key == '1')
        enable_lighting = !enable_lighting;
    else if (key == '2')
        enable_gravity = !enable_gravity;
    else if (key == 27) {  // Escape
        cam_trans_x = 0.0;
        cam_trans_y = 0.0;
        cam_rot_x = 0;
        cam_rot_y = 0;
        cam_rot_z = 0;
        zoom = 1.0;
        enable_gravity = true;
        enable_lighting = true;
    }
}


void key_up(unsigned char key, int x, int y) {
    key_states[key] = false;
}


void special_key_down(int key, int x, int y) {
    special_key_states[key] = true;
}


void special_key_up(int key, int x, int y) {
    special_key_states[key] = false;
}


// ===========
// = PHYSICS =
// ===========


void place_balls() {
    // Place all balls in a circle around the top of the container
    for (int i = 0; i < ball_count; ++i) {
        float angle = 2 * PI * i / ball_count;
        float dist_from_center = 0.99 * (container_radius - ball_radius);
        balls[i] = new Sphere(ball_radius, dist_from_center * cos(angle), 0.8 * container_height, dist_from_center * sin(angle));
    }
}


void physics() {
    for (int i = 0; i < ball_count; ++i) {
        // Handle gravity
        if (enable_gravity)
            balls[i]->velocity->values[1] -= gravity;

        // Handle balls colliding with each other
        for (int j = i + 1; j < ball_count; ++j)
            if (balls[i]->is_touching(balls[j]))
                balls[i]->collide_with(balls[j]);

        // Container spheres
        Sphere *side = new Sphere(container_radius, 0.0, -0.33 * container_height, 0.0);
        side->mass = 10000.0;
        Sphere *bottom = new Sphere(container_radius, 0.0, -0.33 * container_height, 0.0);
        bottom->mass = 10000.0;

        // Handle balls hitting bottom of container
        if (balls[i]->is_touching(bottom)) {
            balls[i]->collide_with(bottom);
            bottom->velocity = new Vector(0.0, 0.0, 0.0);
        }

        // Handle balls hitting side of container w
        Vector *ball_2d = new Vector(balls[i]->location->values[0], balls[i]->location->values[2]);
        Vector *side_2d = new Vector(side->location->values[0], side->location->values[2]);

        if (ball_2d->distance(side_2d) >= side->radius - balls[i]->radius && balls[i]->location->values[1] < container_height - 0.33 * container_height) {
            // Math taken from http://www.3dkingdoms.com/weekly/weekly.php?a=2
            Vector v = Vector(balls[i]->velocity->values[0], balls[i]->velocity->values[2]);
            Vector n = (Vector(0.0, 0.0) - Vector(balls[i]->location->values[0], balls[i]->location->values[2])).normalize();

            Vector v_new = ((n * v.dot(n)) * -2) + v;

            balls[i]->velocity->values[0] = v_new[0];
            balls[i]->velocity->values[2] = v_new[1];
            side->velocity = new Vector(0.0, 0.0, 0.0);
        }

        // Move balls according to their velocity
        balls[i]->location->values[0] += balls[i]->velocity->values[0];
        balls[i]->location->values[1] += balls[i]->velocity->values[1];
        balls[i]->location->values[2] += balls[i]->velocity->values[2];
    }
}


// =======================
// = RENDERING FUNCTIONS =
// =======================


void draw_string(float x, float y, std::string s) {
    glRasterPos2f(x, y);
    for (int i = 0; i < (int)s.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
}


void draw_container() {
    // Render cylinder
    glPolygonStipple(stipple_mask);
    glEnable(GL_POLYGON_STIPPLE);
    GLUquadricObj *quad;
    quad = gluNewQuadric();
    glTranslatef(0.0, -0.33 * container_height, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    gluCylinder(quad, container_radius, container_radius, container_height, 32, 32);
    glRotatef(90.0, 1.0, 0.0, 0.0);

    // Render sphere at bottom of cylinder
    quad = gluNewQuadric();
    gluSphere(quad, container_radius, 32, 32);
    glTranslatef(0.0, 0.33 * container_height, 0.0);

    glDisable(GL_POLYGON_STIPPLE);
}


void draw_balls() {
    for (int i = 0; i < ball_count; ++i) {
        // Cache current color
        float *old_color = new float[3];
        glGetFloatv(GL_CURRENT_COLOR, old_color);

        // Get current ball and color
        Sphere *ball = balls[i];
        GLUquadricObj *quad = gluNewQuadric();
        glColor3f((*ball->color)[0], (*ball->color)[1], (*ball->color)[2]);

        // Draw ball
        glTranslatef((*ball->location)[0], (*ball->location)[1], (*ball->location)[2]);
        gluSphere(quad, ball->radius, 32, 32);
        glTranslatef(-(*ball->location)[0], -(*ball->location)[1], -(*ball->location)[2]);

        // Reset color
        glColor3fv(old_color);
    }
}


void display() {
    double frame_start_time = time();
    buffered_key_operations();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glFrustum(-10, 10, -10, 10, 10, -10);

    // Enable lighting
    glDisable(GL_LIGHTING);
    if (enable_lighting)
        glEnable(GL_LIGHTING);

    // Camera control
    glTranslatef(cam_trans_x, cam_trans_y, 0.0);
    glRotatef(cam_rot_x, 1.0, 0.0, 0.0);
    glRotatef(cam_rot_y, 0.0, 1.0, 0.0);
    glRotatef(cam_rot_z, 0.0, 0.0, 1.0);
    glScalef(zoom, zoom, zoom);

    // Handle physics
    physics();

    // Render container
    draw_container();

    // Draw balls
    draw_balls();

    // Undo camera control
    glScalef(1 / zoom, 1 / zoom, 1 / zoom);
    glRotatef(-cam_rot_z, 0.0, 0.0, 1.0);
    glRotatef(-cam_rot_y, 0.0, 1.0, 0.0);
    glRotatef(-cam_rot_x, 1.0, 0.0, 0.0);
    glTranslatef(-cam_trans_x, -cam_trans_y, 0.0);

    // Calculate framerate
    for (int i = 1; i < framerate_average_count; ++i)
        frame_times[i] = frame_times[i + 1];
    frame_times[4] = time() - frame_start_time;
    double framerate = framerate_average_count / (frame_times[0] + frame_times[1] + frame_times[2] + frame_times[3] + frame_times[4]);

    // Render settings text
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glColor3f(WHITE.r, WHITE.g, WHITE.b);
    draw_string(-0.99, 0.97, std::to_string((int)framerate) + " FPS");
    draw_string(-0.99, 0.94, enable_lighting ? "[1] Lighting: enabled" : "[1] Lighting: disabled");
    draw_string(-0.99, 0.91, enable_gravity ? "[2] gravity: enabled" : "[2] gravity: disabled");
    draw_string(-0.99, 0.88, "[a/d] Camera X Translation: " + std::to_string(cam_trans_x));
    draw_string(-0.99, 0.85, "[w/s] Camera Y Translation: " + std::to_string(cam_trans_y));
    draw_string(-0.99, 0.82, "[i/p] Camera X Rotation: " + std::to_string(cam_rot_x));
    draw_string(-0.99, 0.79, "[o/l] Camera Y Rotation: " + std::to_string(cam_rot_y));
    draw_string(-0.99, 0.76, "[j/;] Camera Z Rotation: " + std::to_string(cam_rot_z));
    draw_string(-0.99, 0.73, "[e/q] Camera Zoom: " + std::to_string(zoom));
    draw_string(-0.99, 0.70, "[ESC] Reset");
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}


int main(int argc, char **argv) {
    srand(time(NULL));

    // Initialize framerate average array
    for (int i = 0; i < framerate_average_count; ++i)
        frame_times[i] = 0.0;

    // Fill list of balls
    place_balls();

    // Set up window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(DISPLAY_SIZE, DISPLAY_SIZE);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("[floating] CEG 4510 Project 4");
    glutDisplayFunc(display);
    glutIdleFunc(glutPostRedisplay);

    // Set up lighting
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    GLfloat lightLocation[4] = {-1, 1, 1, 1};
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
    glDepthFunc(GL_LESS);

    // Callback functions
    glutKeyboardFunc(key_down);
    glutKeyboardUpFunc(key_up);
    glutSpecialFunc(special_key_down);
    glutSpecialUpFunc(special_key_up);

    glutMainLoop();

    // Exit
    delete[] key_states;
    delete[] special_key_states;
    delete[] frame_times;
    return 0;
}
