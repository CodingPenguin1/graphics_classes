#include <GL/glut.h>
#include <chrono>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define DISPLAY_SIZE 1000
#define PI 3.14159265
#define GRID_SIZE 10

struct point {
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
};

struct color {
    float r;
    float g;
    float b;
};

struct force {
    float x;
    float y;
    float z;
};

struct vec3 {
    float a;
    float b;
    float c;
};


// Grid setup
point points[GRID_SIZE][GRID_SIZE];
int selected_row = GRID_SIZE - 1;
int selected_col = 0;

// Physics variables
const float MASS = 100.0;
const float L_REST = 0.125;
const float DIAG_L_REST = sqrtf(2 * pow(L_REST, 2));
const float Ks = 0.1;
const float Kd = 0.1;
const float fric = 0.1;
const int user_force = 1;

// Render variables
const float POINT_RADIUS = 0.01;
int bezier_resolution = GRID_SIZE;
const color WHITE = color{1.0, 1.0, 1.0};
const color GRAY = color{0.5, 0.5, 0.5};
const color YELLOW = color{1.0, 1.0, 0.0};

// Camera controls
int camera_rotation_x = 0;
int camera_rotation_y = 0;
int camera_rotation_z = 0;

// Toggles
bool render_plane = true;
bool enable_physics = true;
bool enable_lighting = true;
bool render_curves = true;

// Misc variables
double start_time;
bool *key_states = new bool[256];
bool *special_key_states = new bool[256];
const int framerate_average_count = 5;
double *frame_times = new double[framerate_average_count];


float map(float val, float old_start, float old_end, float new_start, float new_end) {
    return (val - old_start) / (old_end - old_start) * (new_end - new_start) + new_start;
}


double time() {
    return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();
}


vec3 cross(vec3 a, vec3 b) {
    float x = a.b * b.c - a.c * b.b;
    float y = a.c * b.a - a.a * b.c;
    float z = a.a * b.b - a.b * b.a;
    return vec3{x, y, z};
}


// =====================
// = PHYSICS FUNCTIONS =
// =====================


float distance(point a, point b) {
    return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2) + powf(a.z - b.z, 2));
}


float rad(float degrees) {
    return 180 * degrees / PI;
}


vec3 velocity_difference(point a, point b) {
    return vec3{b.vx - a.vx, b.vy - a.vy, b.vz - a.vz};
}


float dot(vec3 a, vec3 b) {
    return a.a * b.a + a.b * b.b + a.c * b.c;
}


vec3 position_difference(point a, point b) {
    return vec3{b.x - a.x, b.y - a.y, b.z - a.z};
}


vec3 multiply_vector(vec3 a, float scalar) {
    return vec3{a.a * scalar, a.b * scalar, a.c * scalar};
}


vec3 get_centerpoint() {
    vec3 centerpoint = vec3{0.0, 0.0, 0.0};
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            centerpoint.a += points[row][col].x;
            centerpoint.b += points[row][col].y;
            centerpoint.c += points[row][col].z;
        }
    }
    centerpoint.a /= (float)pow(GRID_SIZE, 2);
    centerpoint.b /= (float)pow(GRID_SIZE, 2);
    centerpoint.c /= (float)pow(GRID_SIZE, 2);
    return centerpoint;
}


force get_force(int row, int col) {
    // Calculate individual forces for each spring
    float force_x = 0;
    float force_y = 0;
    float force_z = 0;

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if ((abs(i - row) == 1 && abs(j - col) <= 1) || (abs(j - col) == 1 && abs(i - row) <= 1)) {
                // If horizontal/vertical spring
                if (abs(i - row) != abs(j - col)) {
                    float l_current = distance(points[row][col], points[i][j]);
                    float v_spring = sqrtf(powf(points[row][col].vx - points[i][j].vx, 2) + powf(points[row][col].vy - points[i][j].vy, 2) + powf(points[row][col].vz - points[i][j].vz, 2));
                    float f_magnitude = Ks * (l_current - L_REST);
                    vec3 s_hat = multiply_vector(position_difference(points[row][col], points[i][j]), 1 / l_current);
                    vec3 damping_force = multiply_vector(s_hat, dot(velocity_difference(points[row][col], points[i][j]), s_hat));
                    force_x += (points[row][col].x - points[i][j].x) * f_magnitude / distance(points[row][col], points[i][j]) - Kd * damping_force.a;
                    force_y += (points[row][col].y - points[i][j].y) * f_magnitude / distance(points[row][col], points[i][j]) - Kd * damping_force.b;
                    force_z += (points[row][col].z - points[i][j].z) * f_magnitude / distance(points[row][col], points[i][j]) - Kd * damping_force.c;
                }
                // If diagonal spring
                else {
                    float l_current = distance(points[row][col], points[i][j]);
                    float v_spring = sqrtf(powf(points[row][col].vx - points[i][j].vx, 2) + powf(points[row][col].vy - points[i][j].vy, 2) + powf(points[row][col].vz - points[i][j].vz, 2));
                    float f_magnitude = Ks * (l_current - DIAG_L_REST);
                    vec3 s_hat = multiply_vector(position_difference(points[row][col], points[i][j]), 1 / l_current);
                    vec3 damping_force = multiply_vector(s_hat, dot(velocity_difference(points[row][col], points[i][j]), s_hat));
                    force_x += (points[row][col].x - points[i][j].x) * f_magnitude / distance(points[row][col], points[i][j]) - Kd * damping_force.a;
                    force_y += (points[row][col].y - points[i][j].y) * f_magnitude / distance(points[row][col], points[i][j]) - Kd * damping_force.b;
                    force_z += (points[row][col].z - points[i][j].z) * f_magnitude / distance(points[row][col], points[i][j]) - Kd * damping_force.c;
                }
            }
        }
    }

    return force{-force_x - fric * points[row][col].vx, -force_y - fric * points[row][col].vy, -force_z - fric * points[row][col].vz};
}


// ======================
// = KEYPRESS FUNCTIONS =
// ======================


void buffered_key_operations() {
    // Rotation
    camera_rotation_x = (key_states['q']) ? camera_rotation_x + 2 : camera_rotation_x;
    camera_rotation_x = (key_states['a']) ? camera_rotation_x - 2 : camera_rotation_x;
    camera_rotation_y = (key_states['w']) ? camera_rotation_y + 2 : camera_rotation_y;
    camera_rotation_y = (key_states['s']) ? camera_rotation_y - 2 : camera_rotation_y;
    camera_rotation_z = (key_states['e']) ? camera_rotation_z + 2 : camera_rotation_z;
    camera_rotation_z = (key_states['d']) ? camera_rotation_z - 2 : camera_rotation_z;
    if (abs(camera_rotation_x) >= 360) camera_rotation_x %= 360;
    if (abs(camera_rotation_y) >= 360) camera_rotation_y %= 360;
    if (abs(camera_rotation_z) >= 360) camera_rotation_z %= 360;

    // Bezier resolution controls
    if (key_states['=']) bezier_resolution++;
    if (key_states['-']) bezier_resolution--;
    if (bezier_resolution < GRID_SIZE)
        bezier_resolution = GRID_SIZE;

    // Point movement
    if (special_key_states[100] || special_key_states[101] || special_key_states[102] || special_key_states[103]) {
        // Params for gluProject
        GLdouble *modelview = new GLdouble[16];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        GLdouble *projection = new GLdouble[16];
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        GLint *viewport = new GLint[4];
        glGetIntegerv(GL_VIEWPORT, viewport);


        // Convert selected point to screen space
        GLdouble screen_x;
        GLdouble screen_y;
        GLdouble screen_z;
        point p = points[selected_row][selected_col];
        gluProject(p.x, p.y, p.z, modelview, projection, viewport, &screen_x, &screen_y, &screen_z);

        // Modify screenspace coordintes
        // Up
        if (special_key_states[101])
            screen_y += user_force;
        // Down
        if (special_key_states[103])
            screen_y -= user_force;
        // Left
        if (special_key_states[100])
            screen_x -= user_force;
        // Right
        if (special_key_states[102])
            screen_x += user_force;

        // Convert back to world space
        GLdouble world_x;
        GLdouble world_y;
        GLdouble world_z;
        gluUnProject(screen_x, screen_y, screen_z, modelview, projection, viewport, &world_x, &world_y, &world_z);
        points[selected_row][selected_col].x = (float)world_x;
        points[selected_row][selected_col].y = (float)world_y;
        points[selected_row][selected_col].z = (float)world_z;
    }
}


void key_down(unsigned char key, int x, int y) {
    key_states[key] = true;
    // Toggles
    if (key == 'r')
        render_plane = !render_plane;
    else if (key == 'p')
        enable_physics = !enable_physics;
    else if (key == 'l')
        enable_lighting = !enable_lighting;
    else if (key == 'c')
        render_curves = !render_curves;

    // Reset
    else if (key_states['h']) {
        camera_rotation_x = 0;
        camera_rotation_y = 0;
        camera_rotation_z = 0;
        for (int row = 0; row < GRID_SIZE; ++row)
            for (int col = 0; col < GRID_SIZE; ++col)
                points[row][col] = point{col * L_REST - 1.5 * L_REST, row * L_REST - 1.5 * L_REST, 0.0, 0.0, 0.0, 0.0};
    }

    // Point selection
    else if (key_states[9]) {
        selected_col++;
        if (selected_col >= GRID_SIZE) {
            selected_col = 0;
            selected_row--;
            if (selected_row < 0)
                selected_row = GRID_SIZE - 1;
        }
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


// =======================
// = RENDERING FUNCTIONS =
// =======================


void draw_spring(point a, point b, float line_width, bool diagonal) {
    float max_stretch = diagonal ? DIAG_L_REST : L_REST;
    max_stretch *= 4;
    color c = color{distance(a, b), max_stretch - distance(a, b), (distance(a, b) < L_REST) ? 1.0 : 0.0};
    glColor3f(c.r, c.g, c.b);
    glLineWidth(line_width);
    glBegin(GL_LINES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glEnd();
}


void draw_string(float x, float y, std::string s) {
    glRasterPos2f(x, y);
    for (int i = 0; i < (int)s.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
}


vec3 interpolate(vec3 a, vec3 b, float t) {
    vec3 interpolated_point;
    interpolated_point.a = a.a + (t * (b.a - a.a));
    interpolated_point.b = a.b + (t * (b.b - a.b));
    interpolated_point.c = a.c + (t * (b.c - a.c));
    return interpolated_point;
}


vec3 bezier(float t, vec3 *vector_list, int num_vectors) {
    if (num_vectors == 1)
        return vector_list[0];


    vec3 *interpolated_points = new vec3[num_vectors - 1];
    for (int i = 0; i < num_vectors - 1; ++i)
        interpolated_points[i] = interpolate(vector_list[i], vector_list[i + 1], t);
    vec3 point = bezier(t, interpolated_points, num_vectors - 1);
    delete[] interpolated_points;
    return point;
}


vec3 bezier_points(float t, point *point_list, int num_points) {
    // Points from interpolated lines
    vec3 *vector_list = new vec3[num_points];
    for (int i = 0; i < num_points; ++i)
        vector_list[i] = vec3{point_list[i].x, point_list[i].y, point_list[i].z};
    vec3 point = bezier(t, vector_list, num_points);
    delete[] vector_list;
    return point;
}


void display() {
    double frame_start_time = time();
    buffered_key_operations();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-1, 1, -1, 1, 1, -1);

    // Calculate physics
    if (enable_physics) {
        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int col = 0; col < GRID_SIZE; ++col) {
                //Calculate velocities of all points
                force f = get_force(row, col);
                points[row][col].vx += f.x / MASS;
                points[row][col].vy += f.y / MASS;
                points[row][col].vz += f.z / MASS;

                // Calculate new positions of all points
                points[row][col].x += points[row][col].vx;
                points[row][col].y += points[row][col].vy;
                points[row][col].z += points[row][col].vz;
            }
        }
    }

    // Recenter mesh
    vec3 centerpoint = get_centerpoint();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(camera_rotation_x, 1.0, 0.0, 0.0);
    glRotatef(camera_rotation_y, 0.0, 1.0, 0.0);
    glRotatef(camera_rotation_z, 0.0, 0.0, 1.0);
    glTranslatef(-centerpoint.a, -centerpoint.b, -centerpoint.c);

    // Enable lighting
    if (enable_lighting)
        glEnable(GL_LIGHTING);

    // Render straight lines/planes
    if (!render_curves) {
        // Render spring lines
        if (!render_plane) {
            float line_width = POINT_RADIUS / 2.0;
            for (int row = 0; row < GRID_SIZE; ++row) {
                for (int col = 0; col < GRID_SIZE; ++col) {
                    for (int i = 0; i < GRID_SIZE; ++i) {
                        for (int j = 0; j < GRID_SIZE; ++j) {
                            if ((abs(i - row) == 1 && abs(j - col) <= 1) || (abs(j - col) == 1 && abs(i - row) <= 1)) {  // If horizontal/vertical spring
                                if (abs(i - row) != abs(j - col)) {
                                    draw_spring(points[row][col], points[i][j], line_width, false);
                                }
                                // If diagonal spring
                                else {
                                    draw_spring(points[row][col], points[i][j], line_width, true);
                                }
                            }
                        }
                    }
                }
            }
        }
        // Render planes
        else {
            for (int row = 0; row < GRID_SIZE - 1; ++row) {
                for (int col = 0; col < GRID_SIZE - 1; ++col) {
                    glColor3f(1.0, 1.0, 1.0);
                    glBegin(GL_QUADS);
                    glVertex3d(points[row][col].x, points[row][col].y, points[row][col].z);
                    glVertex3d(points[row][col + 1].x, points[row][col + 1].y, points[row][col + 1].z);
                    glVertex3d(points[row + 1][col + 1].x, points[row + 1][col + 1].y, points[row + 1][col + 1].z);
                    glVertex3d(points[row + 1][col].x, points[row + 1][col].y, points[row + 1][col].z);
                    glVertex3d(points[row][col].x, points[row][col].y, points[row][col].z);
                    glEnd();
                }
            }
        }
    }

    // Render Bezier curves/planes
    else {
        // Generate bezier curves on rows
        // Array for saving points
        vec3 **bezier_row = (vec3 **)malloc(GRID_SIZE * sizeof(vec3 *));
        for (int i = 0; i < GRID_SIZE; ++i)
            bezier_row[i] = (vec3 *)malloc(bezier_resolution * sizeof(vec3));

        // Generate points
        point *points_list = new point[GRID_SIZE];
        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int i = 0; i < GRID_SIZE; ++i)
                points_list[i] = points[row][i];
            for (int col = 0; col < bezier_resolution; ++col) {
                float t = (float)col / ((float)bezier_resolution - 1.0);
                bezier_row[row][col] = bezier_points(t, points_list, GRID_SIZE);
            }
        }

        // Generate bezier curves on cols
        // Array for saving points
        vec3 **bezier_points = (vec3 **)malloc(bezier_resolution * sizeof(vec3 *));
        for (int i = 0; i < bezier_resolution; ++i)
            bezier_points[i] = (vec3 *)malloc(bezier_resolution * sizeof(vec3));

        // Generate points
        vec3 *vector_list = new vec3[GRID_SIZE];
        for (int col = 0; col < bezier_resolution; ++col) {
            for (int i = 0; i < GRID_SIZE; ++i)
                vector_list[i] = bezier_row[i][col];
            for (int row = 0; row < bezier_resolution; ++row) {
                float t = (float)row / ((float)bezier_resolution - 1.0);
                bezier_points[row][col] = bezier(t, vector_list, GRID_SIZE);
            }
        }

        // Render splines
        if (!render_plane) {
            glLineWidth(POINT_RADIUS / 2.0);

            // Draw rows
            glColor3f(WHITE.r, WHITE.g, WHITE.b);
            for (int row = 0; row < bezier_resolution; ++row) {
                for (int col = 0; col < bezier_resolution - 1; ++col) {
                    glBegin(GL_LINES);
                    glVertex3f(bezier_points[row][col].a, bezier_points[row][col].b, bezier_points[row][col].c);
                    glVertex3f(bezier_points[row][col + 1].a, bezier_points[row][col + 1].b, bezier_points[row][col + 1].c);
                    glEnd();
                }
            }

            // Draw columns
            glColor3f(WHITE.r, WHITE.g, WHITE.b);
            for (int col = 0; col < bezier_resolution; ++col) {
                for (int row = 0; row < bezier_resolution - 1; ++row) {
                    glBegin(GL_LINES);
                    glVertex3f(bezier_points[row][col].a, bezier_points[row][col].b, bezier_points[row][col].c);
                    glVertex3f(bezier_points[row + 1][col].a, bezier_points[row + 1][col].b, bezier_points[row + 1][col].c);
                    glEnd();
                }
            }
        }

        // Render "curved" planes
        else {
            for (int row = 0; row < bezier_resolution - 1; ++row) {
                for (int col = 0; col < bezier_resolution - 1; ++col) {
                    // Corners
                    vec3 a = bezier_points[row][col];
                    vec3 b = bezier_points[row][col + 1];
                    vec3 c = bezier_points[row + 1][col];
                    vec3 d = bezier_points[row + 1][col + 1];

                    // Bottom left triangle
                    glBegin(GL_TRIANGLES);
                    glVertex3f(a.a, a.b, a.c);
                    glVertex3f(b.a, b.b, b.c);
                    glVertex3f(c.a, c.b, c.c);
                    vec3 u = vec3{b.a - a.a, b.b - a.b, b.c - a.c};
                    vec3 v = vec3{c.a - a.a, c.b - a.b, c.c - a.c};
                    vec3 normal = cross(u, v);
                    glNormal3f(normal.a, normal.b, normal.c);
                    glEnd();

                    // Top right triangle
                    glBegin(GL_TRIANGLES);
                    glVertex3f(b.a, b.b, b.c);
                    glVertex3f(c.a, c.b, c.c);
                    glVertex3f(d.a, d.b, d.c);
                    u = vec3{b.a - d.a, b.b - d.b, b.c - d.c};
                    v = vec3{c.a - d.a, c.b - d.b, c.c - d.c};
                    normal = cross(u, v);
                    glNormal3f(-normal.a, -normal.b, -normal.c);
                    glEnd();
                }
            }
        }

        // Array cleanup
        delete[] points_list;
        delete[] vector_list;
        for (int i = 0; i < GRID_SIZE; ++i)
            free(bezier_row[i]);
        free(bezier_row);
        for (int i = 0; i < bezier_resolution; ++i)
            free(bezier_points[i]);
        free(bezier_points);
    }

    // Disable lighting before drawing text
    if (enable_lighting)
        glDisable(GL_LIGHTING);

    // Draw the circles
    glPointSize(5);
    glBegin(GL_POINTS);
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            color c = color{0.0, 0.0, 1.0};
            if (row == selected_row && col == selected_col)
                c = YELLOW;
            glColor3f(c.r, c.g, c.b);
            glVertex3f(points[row][col].x, points[row][col].y, points[row][col].z);
        }
    }
    glEnd();

    // Calculate framerate
    for (int i = 1; i < framerate_average_count; ++i)
        frame_times[i] = frame_times[i + 1];
    frame_times[4] = time() - frame_start_time;
    double framerate = framerate_average_count / (frame_times[0] + frame_times[1] + frame_times[2] + frame_times[3] + frame_times[4]);

    // Render settings text
    glPushMatrix();
    glLoadIdentity();
    glColor3f(WHITE.r, WHITE.g, WHITE.b);
    if (enable_physics)
        draw_string(-0.99, 0.97, "[p] Physics: enabled");
    else
        draw_string(-0.99, 0.97, "[p] Physics: disabled");
    if (render_plane)
        draw_string(-0.99, 0.94, "[r] Render mode: plane");
    else
        draw_string(-0.99, 0.94, "[r] Render mode: springs");
    if (enable_lighting)
        draw_string(-0.99, 0.91, "[l] Lighting: enabled");
    else
        draw_string(-0.99, 0.91, "[l] Lighting: disabled");
    if (render_curves)
        draw_string(-0.99, 0.88, "[c] Render curves: enabled");
    else
        draw_string(-0.99, 0.88, "[c] Render curves: disabled");
    draw_string(-0.99, 0.85, "[q/a] X Rotation: " + std::to_string(camera_rotation_x));
    draw_string(-0.99, 0.82, "[w/s] Y Rotation: " + std::to_string(camera_rotation_y));
    draw_string(-0.99, 0.79, "[e/d] Z Rotation: " + std::to_string(camera_rotation_z));
    draw_string(-0.99, 0.76, "[TAB] Cycle selected point");
    draw_string(-0.99, 0.73, "[UP] Move selected point up");
    draw_string(-0.99, 0.70, "[DOWN] Move selected point down");
    draw_string(-0.99, 0.67, "[LEFT] Move selected point left");
    draw_string(-0.99, 0.64, "[RIGHT] Move selected point right");
    draw_string(-0.99, 0.61, "[=/-] Bezier resolution: " + std::to_string(bezier_resolution));
    draw_string(-0.99, 0.58, "[h] Reset");
    draw_string(-0.99, 0.55, std::to_string((int)framerate) + " FPS");
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}


int main(int argc, char **argv) {
    // Set start time of program
    start_time = time();
    printf("Start time: %f\n", start_time);

    // Create points array
    for (int row = 0; row < GRID_SIZE; ++row)
        for (int col = 0; col < GRID_SIZE; ++col)
            points[row][col] = point{col * L_REST - 1.5 * L_REST, row * L_REST - 1.5 * L_REST, 0.0, 0.0, 0.0, 0.0};

    // Initialize framerate average array
    for (int i = 0; i < framerate_average_count; ++i)
        frame_times[i] = 0.0;

    // Set up window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(DISPLAY_SIZE, DISPLAY_SIZE);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("[floating] CEG 4510 Project 2");
    glutDisplayFunc(display);
    glutIdleFunc(glutPostRedisplay);

    // Set up lighting
    glEnable(GL_NORMALIZE);
    // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    GLfloat lightLocation[4] = {0, 0, 1, 1};
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
