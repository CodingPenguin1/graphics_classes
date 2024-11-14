#include <GL/glut.h>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <thread>

#include "ply.h"
#include "vector.h"


#define MOUSE_LEFT_BUTTON 0
#define MOUSE_MIDDLE_BUTTON 1
#define MOUSE_RIGHT_BUTTON 2
#define MOUSE_SCROLL_UP 3
#define MOUSE_SCROLL_DOWN 4

#define NO_MODIFIERS
#define SHIFT_PRESSED 1
#define CTRL_PRESSED 2
#define CTRL_SHIFT_PRESSED 3
#define ALT_PRESSED 4
#define ALT_SHIFT_PRESSED 5
#define ALT_CTRL_PRESSED 6
#define ALT_CTRL_SHIFT_PRESSED 7

#define PI 3.14159265

#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 300


// Object global vars
int num_elems;
Vertex **vlist;
Face **flist;

// Location/camera global vars
float x_offset = 0.0;
float y_offset = 0.0;
float zoom = 1.0;
float horizontal_angle = 0.0;
float vertical_angle = 0.0;
float old_horizontal_angle = 0.0;
float old_vertical_angle = 0.0;
int mouse_anchor_x = 999999999;
int mouse_anchor_y = 999999999;
std::map<char, uint> keys = {
        {'w', 0},
        {'a', 0},
        {'s', 0},
        {'d', 0},
};

// Other global variables
bool ray_traced = false;
bool save_ppm = false;  // Used when handling keyboard callback

// Illumination constants
const float ka = 0.3;
const float kd = 0.3;
const float ks = 0.4;
const float n = 50;
const float Ii = 1;
const float Ia = 1;
const Vector base_color = Vector(0.5, 0, 1);

// Raytracing is multiprocessed. This is the args for each thread as a single parameter
struct thread_parameter {
    int thread_id;
    uint row_min;
    uint row_max;
    Vector camera = Vector(0, 0, 0);
    float *colors;
    float fuzz_value;
};


void read_ply(char *filename) {
    int i, j;
    PlyFile *ply;
    int nelems;
    char **elist;
    int file_type;
    float version;
    int nprops;
    PlyProperty **plist;
    char *elem_name;

    /* open a PLY file for reading */
    ply = ply_open_for_reading(filename, &nelems, &elist, &file_type, &version);

    /* go through each kind of element that we learned is in the file */
    /* and read them */
    for (i = 0; i < nelems; i++) {
        /* get the description of the first element */
        elem_name = elist[i];
        plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);

        /* if we're on vertex elements, read them in */
        if (equal_strings((char *)"vertex", elem_name)) {
            /* create a vertex list to hold all the vertices */
            vlist = (Vertex **)malloc(sizeof(Vertex *) * num_elems);

            /* set up for getting vertex elements */
            ply_get_property(ply, elem_name, &vert_props[0]);
            ply_get_property(ply, elem_name, &vert_props[1]);
            ply_get_property(ply, elem_name, &vert_props[2]);

            /* grab all the vertex elements */
            for (j = 0; j < num_elems; j++) {
                /* grab and element from the file */
                vlist[j] = (Vertex *)malloc(sizeof(Vertex));
                ply_get_element(ply, (void *)vlist[j]);
            }
        }

        /* if we're on face elements, read them in */
        if (equal_strings((char *)"face", elem_name)) {
            /* create a list to hold all the face elements */
            flist = (Face **)malloc(sizeof(Face *) * num_elems);

            /* set up for getting face elements */
            ply_get_property(ply, elem_name, &face_props[0]);
            ply_get_property(ply, elem_name, &face_props[1]);

            /* grab all the face elements */
            for (j = 0; j < num_elems; j++) {
                /* grab and element from the file */
                flist[j] = (Face *)malloc(sizeof(Face));
                ply_get_element(ply, (void *)flist[j]);
            }
        }
    }

    /* close the PLY file */
    ply_close(ply);
}


// Maps a value from one range to another
float map(float value, float old_min, float old_max, float new_min, float new_max) {
    return (value - old_min) / (old_max - old_min) * (new_max - new_min) + new_min;
}


// Returns the direction of the normal vector of a plane defined by 3 points
Vector get_normal(Vector a, Vector b, Vector c) {
    return ((b - a).cross(c - a));
}


void write_ppm(float *colors) {
    std::cout << "Writing window.ppm" << std::endl;
    // Open file
    std::ofstream file("window.ppm");

    // Write header
    file << "P6" << std::endl;
    file << "# CEG 4500 Project 4" << std::endl;
    file << "# Creator: Ryan J Slater" << std::endl;
    file << WINDOW_WIDTH << " " << WINDOW_HEIGHT << std::endl;
    file << "255" << std::endl;

    // Write data
    for (int i = WINDOW_WIDTH * WINDOW_HEIGHT * 3 - 1; i - 2 >= 0; i -= 3) {
        uint red = colors[i - 2] * 255;
        uint green = colors[i - 1] * 255;
        uint blue = colors[i] * 255;
        file.put((char)red);
        file.put((char)green);
        file.put((char)blue);
    }

    // Close file
    file.close();
    std::cout << "Done" << std::endl;
}


void handle_keys() {
    if (keys['w'])
        y_offset += 0.01;
    if (keys['a'])
        x_offset -= 0.01;
    if (keys['s'])
        y_offset -= 0.01;
    if (keys['d'])
        x_offset += 0.01;
}


void render_faces() {
    glBegin(GL_TRIANGLES);
    glColor3f(base_color.x, base_color.y, base_color.z);
    for (int face_index = 0; face_index < num_elems; ++face_index) {
        Face *face = flist[face_index];
        glVertex3f(vlist[face->verts[0]]->x, vlist[face->verts[0]]->y, vlist[face->verts[0]]->z);
        glVertex3f(vlist[face->verts[1]]->x, vlist[face->verts[1]]->y, vlist[face->verts[1]]->z);
        glVertex3f(vlist[face->verts[2]]->x, vlist[face->verts[2]]->y, vlist[face->verts[2]]->z);
    }
    glEnd();
}


void *render_cell(void *thread_arg) {
    // Unpack thread parameters
    struct thread_parameter *my_data;
    my_data = (struct thread_parameter *)thread_arg;
    uint thread_id = my_data->thread_id;
    uint row_min = my_data->row_min;
    uint row_max = my_data->row_max;
    Vector camera = my_data->camera;
    float *colors = my_data->colors;
    float fuzz_value = my_data->fuzz_value;

    // Iterate through each pixel in the thread's assigned cell
    for (uint row = row_min; row < row_max; ++row) {
        for (uint col = 0; col < WINDOW_HEIGHT; ++col) {
            // Direction for ray shot from camera towards the pixel at (row, col)
            Vector ray = Vector(map(col, 0, WINDOW_WIDTH, -0.2, 0.2) + (old_horizontal_angle + horizontal_angle) / 360, map(row, 0, WINDOW_HEIGHT, -0.2, 0.2) - (old_vertical_angle + vertical_angle) / 360, 1);
            bool intersects = false;

            // Used for Phong illumination
            Face *closest_face = NULL;
            Vector closest_intersection_point = Vector(0, 0, 0);
            Vector closest_normal = Vector(0, 0, 0);
            float distance = 100000000;

            for (uint face_index = 0; face_index < num_elems; ++face_index) {
                Face *face = flist[face_index];

                // Define the intersection plane
                Vector point_a = Vector(vlist[face->verts[0]]->x, vlist[face->verts[0]]->y, vlist[face->verts[0]]->z);
                Vector point_b = Vector(vlist[face->verts[1]]->x, vlist[face->verts[1]]->y, vlist[face->verts[1]]->z);
                Vector point_c = Vector(vlist[face->verts[2]]->x, vlist[face->verts[2]]->y, vlist[face->verts[2]]->z);
                Vector normal = get_normal(point_a, point_b, point_c).normalize();

                // Calculate time factor
                float t = ((point_a - camera) * normal) / (ray * normal);

                // Calculate the intersection point
                Vector intersection_point = camera + ray * t;

                // Calculate the areas of each triangle
                Vector a_delta = point_a - intersection_point;
                Vector b_delta = point_b - intersection_point;
                Vector c_delta = point_c - intersection_point;
                float area_0 = 0.5 * ((a_delta).cross(b_delta)).magnitude();
                float area_1 = 0.5 * ((a_delta).cross(c_delta)).magnitude();
                float area_2 = 0.5 * ((b_delta).cross(c_delta)).magnitude();
                float area_face = 0.5 * ((point_a - point_b).cross(point_c - point_b)).magnitude();

                // Determine if the ray for this pixel intersects a surface or not
                if (area_0 + area_1 + area_2 < area_face + fuzz_value) {
                    // If so, check only save the face if it's the closest face
                    // No point shading a face that we won't see
                    if (t < distance && t < 10000) {  // distance should be close to camera to prevent random pixels being generated
                        distance = t;
                        closest_face = face;
                        closest_intersection_point = intersection_point;
                        closest_normal = normal;
                    }
                }
            }

            // Phong illumination
            if (closest_face != nullptr) {
                Vector light_ray = (camera - closest_intersection_point).normalize();
                Vector reflection = closest_normal * (2 * (closest_normal * light_ray)) - light_ray;
                float intensity = Ii * (kd * (light_ray * closest_normal) + ks * pow(reflection * light_ray, n)) + ka * Ia;

                // Set the colors
                colors[3 * (row * WINDOW_HEIGHT + col)] = abs((1 - intensity) * base_color.x);
                colors[3 * (row * WINDOW_HEIGHT + col) + 1] = abs((1 - intensity) * base_color.y);
                colors[3 * (row * WINDOW_HEIGHT + col) + 2] = abs((1 - intensity) * base_color.z);
                if (colors[3 * (row * WINDOW_HEIGHT + col)] > 1) colors[3 * (row * WINDOW_HEIGHT + col)] = 1;
                if (colors[3 * (row * WINDOW_HEIGHT + col) + 1] > 1) colors[3 * (row * WINDOW_HEIGHT + col) + 1] = 1;
                if (colors[3 * (row * WINDOW_HEIGHT + col) + 2] > 1) colors[3 * (row * WINDOW_HEIGHT + col) + 2] = 1;
            }
        }
    }

    pthread_exit(0);
}


float *render_ray_traced(float fuzz_value = 0.0000001) {
    // Camera vector and pixel array
    Vector camera = Vector(x_offset * 5, y_offset * 5, -30.0 / zoom);
    float *colors = new float[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    for (uint i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT * 3; ++i)
        colors[i] = 0;

    // Calculate how many threads to run (how many 100x100px blocks)
    const uint cpu_count = std::thread::hardware_concurrency();
    const uint cell_height = WINDOW_HEIGHT / cpu_count;

    pthread_t threads[cpu_count];
    struct thread_parameter thread_parameters[cpu_count];
    int rc;

    // Generate squares of pixels, starting from the top and working down
    for (uint thread_id = 0; thread_id < cpu_count; ++thread_id) {
        thread_parameters[thread_id].thread_id = thread_id;

        // Cell dimensions
        thread_parameters[thread_id].row_min = thread_id * cell_height;
        thread_parameters[thread_id].row_max = (thread_id + 1) * cell_height;

        // Other thread data
        thread_parameters[thread_id].camera = camera;
        thread_parameters[thread_id].colors = colors;
        thread_parameters[thread_id].fuzz_value = fuzz_value;

        // Create the threads
        rc = pthread_create(&threads[thread_id], NULL, render_cell, (void *)&thread_parameters[thread_id]);
        if (rc != 0) {
            if (rc == EAGAIN)
                std::cout << "System thread limit exceeded" << std::endl;
            else if (rc == EINVAL)
                std::cout << "Value of thread attr is invalid" << std::endl;
            else if (rc == EPERM)
                std::cout << "Caller does not have appropriate permissions for scheduling threads" << std::endl;
            else
                std::cout << "Something done broke bad" << std::endl;
            exit(-1);
        }
    }

    // Wait for threads to finish
    for (uint i = 0; i < cpu_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Display the frame and reset
    glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, colors);
    delete[] colors;
    glEnd();
    return colors;
}


void draw() {
    // Timer
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    // Initialization
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_DEPTH_BUFFER_BIT);
    glFrustum(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    // Process any pressed keys
    handle_keys();

    // Render frame
    if (ray_traced) {
        float *colors = render_ray_traced();
        if (save_ppm) {
            write_ppm(colors);
            save_ppm = false;
        }
    } else {
        glRotatef(-(old_vertical_angle + vertical_angle), 1.0, 0.0, 0);
        glRotatef(old_horizontal_angle + horizontal_angle, 0.0, 1.0, 0.0);
        glTranslatef(-x_offset, -y_offset, 0.0);
        if (zoom != 1) glScalef(zoom, zoom, zoom);
        render_faces();
        if (zoom != 1) glScalef(1 / zoom, 1 / zoom, 1 / zoom);
        glTranslatef(x_offset, y_offset, 0.0);
        glRotatef(-(old_horizontal_angle + horizontal_angle), 0.0, 1.0, 0.0);
        glRotatef(old_vertical_angle + vertical_angle, 1.0, 0.0, 0);
    }

    // Display frame
    glFlush();
    glutSwapBuffers();

    // FPS counter
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << (int)(1 / elapsed_seconds.count()) << " FPS" << std::endl;
}


void click_callback(int button, int state, int x, int y) {
    if (button == 0 && state == 0) {
        mouse_anchor_x = x;
        mouse_anchor_y = y;
        old_horizontal_angle += horizontal_angle;
        old_vertical_angle += vertical_angle;
    } else if (button == 3)
        zoom *= 1.05;
    else if (button == 4)
        zoom *= 0.95;
}


void motion_callback(int x, int y) {
    if (mouse_anchor_x == 999999999 || mouse_anchor_y == 999999999) {
        // Set initial values for previous mouse coords
        mouse_anchor_x = x;
        mouse_anchor_y = y;
    } else {
        // Otherwise, rotate the object accordingly
        horizontal_angle = (x - mouse_anchor_x) % 360;
        vertical_angle = (y - mouse_anchor_y) % 360;
    }
}


void key_down(unsigned char key, int x, int y) {
    if (key == ' ')
        save_ppm = true;
    else if (key != 'r')
        keys[key] = 1;
    else
        ray_traced = !ray_traced;
}


void key_up(unsigned char key, int x, int y) {
    keys[key] = 0;
}


int main(int argc, char **argv) {
    if (argc > 1) {
        // Read PLY file
        char *filename = argv[1];
        std::cout << "Loading file: " << filename << std::endl;
        read_ply(filename);

        // If not all faces are triangles, error out
        for (int i = 0; i < num_elems; ++i) {
            uint num_verticies = flist[i]->nverts;
            if (num_verticies != 3) {
                std::cout << "ERROR: Found face with " << num_verticies << " verticies (expected 3)" << std::endl;
                return 1;
            }
        }

        // Initialize glut
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowPosition(0, 0);
        glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        glutCreateWindow("CEG4500 Project4");

        // Action functions
        glutMouseFunc(click_callback);
        glutMotionFunc(motion_callback);
        glutKeyboardFunc(key_down);
        glutKeyboardUpFunc(key_up);
        glutDisplayFunc(draw);
        glutIdleFunc(glutPostRedisplay);
        glutMainLoop();
    }
}
