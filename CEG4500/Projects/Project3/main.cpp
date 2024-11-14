#include <GL/glut.h>
#include <chrono>
#include <fstream>
#include <iostream>

// Array for storing intensity values
const int max_x = 289;
const int max_y = 240;
const int max_z = 267;
const int num_points = max_x * max_y * max_z;
uint8_t intensities[num_points];

// Min/max rgb values (for mapping intensities to colors)
// float min_red = 0.2;
// float min_green = 0.2;
// float min_blue = 0.2;
// float max_red = 1;
// float max_green = 1;
// float max_blue = 1;
float min_red = 240.0 / 255.0;
float min_green = 233.0 / 255.0;
float min_blue = 105.0 / 255.0;
float max_red = 107.0 / 255.0;
float max_green = 168.0 / 255.0;
float max_blue = 194.0 / 255.0;

// Slider values
float x_slider_value = 0;
float y_slider_value = 0;
float z_slider_value = 0;
float scale_slider_value = 0;

auto start = std::chrono::high_resolution_clock::now();


bool read_raw(char *filename) {
    FILE *ptr;
    ptr = fopen(filename, "rb");
    fread(intensities, sizeof(intensities), 1, ptr);
    return true;
}


float map(float old_min, float old_max, float new_min, float new_max, float value) {
    return new_min + ((value - old_min) * (new_max - new_min)) / (old_max - old_min);
}


void intensity_to_color(uint8_t intensity, float *color) {
    color[0] = map(0, 255, min_red, max_red, intensity);
    color[1] = map(0, 255, min_green, max_green, intensity);
    color[2] = map(0, 255, min_blue, max_blue, intensity);
}


void draw_points() {
    glPointSize(2);
    glBegin(GL_POINTS);
    int x = 0;
    int y = 0;
    int z = 0;
    float current_color[3];

    // Render scale
    float zoom_factor = map(-1, 1, 0, 600, scale_slider_value);

    for (int i = 0; i < num_points; ++i) {
        if ((int)intensities[i] > 0) {
            intensity_to_color((int)intensities[i], current_color);
            glColor3f(current_color[0], current_color[1], current_color[2]);
            glVertex3f((x - (max_x / 2)) / zoom_factor, (y - (max_y / 2)) / zoom_factor, (z - (max_z / 2)) / zoom_factor);
        }

        x++;
        if (x >= max_x) {
            x = 0;
            y++;
        }
        if (y >= max_y) {
            y = 0;
            z++;
        }
    }
    glEnd();
}


float slider_value_to_angle(float slider_value) {
    return map(-1, 1, -180, 180, slider_value);
}


float slider_to_coordinate(float min_coord, float max_coord, float slider_value) {
    // Map slider value of -1 to min, +1 to max
    return map(-1, 1, min_coord, max_coord, slider_value);
}


void draw_rect(float left, float right, float top, float bottom, float z, float red, float green, float blue) {
    glColor3f(red, green, blue);
    glBegin(GL_QUADS);
    glVertex3f(left, top, z);
    glVertex3f(right, top, z);
    glVertex3f(right, bottom, z);
    glVertex3f(left, bottom, z);
    glEnd();
}


void draw() {
    // Initialization
    glClearColor(0, 0, 0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_DEPTH_BUFFER_BIT);
    glFrustum(-1, 1, -1, 1, -1, 1);

    // Get rotation offsets from sliders
    float x_angle_offset = slider_value_to_angle(x_slider_value);
    float y_angle_offset = slider_value_to_angle(y_slider_value);
    float z_angle_offset = slider_value_to_angle(z_slider_value);

    // Draw points
    glTranslatef(-0.5, 0, 0);
    glRotatef(x_angle_offset, 1, 0, 0);
    glRotatef(y_angle_offset, 0, 1, 0);
    glRotatef(z_angle_offset, 0, 0, 1);
    draw_points();
    glRotatef(-z_angle_offset, 0, 0, 1);
    glRotatef(-y_angle_offset, 0, 1, 0);
    glRotatef(-x_angle_offset, 1, 0, 0);
    glTranslatef(0.5, 0, 0);

    // Draw back of sliders
    float slider_min_coord = -0.15;
    float slider_max_coord = -0.85;
    draw_rect(0.3, 0.35, slider_min_coord + 0.05, slider_max_coord - 0.05, 0, 0.4, 0, 0);    // X
    draw_rect(0.4, 0.45, slider_min_coord + 0.05, slider_max_coord - 0.05, 0, 0, 0.4, 0);    // Y
    draw_rect(0.5, 0.55, slider_min_coord + 0.05, slider_max_coord - 0.05, 0, 0, 0, 0.4);    // Z
    draw_rect(0.6, 0.65, slider_min_coord + 0.05, slider_max_coord - 0.05, 0, 0.4, 0, 0.4);  // Scale

    // Draw slider buttons
    float x_coord = slider_to_coordinate(slider_min_coord, slider_max_coord, x_slider_value);
    float y_coord = slider_to_coordinate(slider_min_coord, slider_max_coord, y_slider_value);
    float z_coord = slider_to_coordinate(slider_min_coord, slider_max_coord, z_slider_value);
    float s_coord = slider_to_coordinate(slider_min_coord, slider_max_coord, scale_slider_value);
    draw_rect(0.3, 0.35, x_coord + 0.05, x_coord - 0.05, -0.1, 1, 0, 0);  // X
    draw_rect(0.4, 0.45, y_coord + 0.05, y_coord - 0.05, -0.1, 0, 1, 0);  // Y
    draw_rect(0.5, 0.55, z_coord + 0.05, z_coord - 0.05, -0.1, 0, 0, 1);  // Z
    draw_rect(0.6, 0.65, s_coord + 0.05, s_coord - 0.05, -0.1, 1, 0, 1);  // Scale

    // Update display
    glFlush();
    glutSwapBuffers();

    // FPS counter
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << 1000000 / duration.count() << " fps" << std::endl;
    start = std::chrono::high_resolution_clock::now();
}


void slider_move(int x, int y) {
    float window_width = glutGet(GLUT_WINDOW_WIDTH);
    float window_height = glutGet(GLUT_WINDOW_HEIGHT);

    float x_coord_scale = (0.05 - (-1)) / (1 - (-1)) * window_width;
    float x_slider_x_coord_min = window_width / 2 + 0.28 * x_coord_scale;
    float x_slider_x_coord_max = window_width / 2 + 0.35 * x_coord_scale;
    float y_slider_x_coord_min = window_width / 2 + 0.38 * x_coord_scale;
    float y_slider_x_coord_max = window_width / 2 + 0.45 * x_coord_scale;
    float z_slider_x_coord_min = window_width / 2 + 0.48 * x_coord_scale;
    float z_slider_x_coord_max = window_width / 2 + 0.55 * x_coord_scale;
    float s_slider_x_coord_min = window_width / 2 + 0.58 * x_coord_scale;
    float s_slider_x_coord_max = window_width / 2 + 0.65 * x_coord_scale;

    float y_coord_min = window_height - (-0.15 - (-1)) / (1 - (-1)) * window_height;
    float y_coord_max = window_height - (-0.85 - (-1)) / (1 - (-1)) * window_height;

    float slider_min = -1;
    float slider_max = 1;

    // If over x slider
    if (x >= x_slider_x_coord_min && x <= x_slider_x_coord_max && y >= y_coord_min && y <= y_coord_max) {
        x_slider_value = (y - y_coord_min) / (y_coord_max - y_coord_min) * (slider_max - slider_min) + slider_min;
        // If over y slider
    } else if (x >= y_slider_x_coord_min && x <= y_slider_x_coord_max && y >= y_coord_min && y <= y_coord_max) {
        y_slider_value = (y - y_coord_min) / (y_coord_max - y_coord_min) * (slider_max - slider_min) + slider_min;
        // If over z slider
    } else if (x >= z_slider_x_coord_min && x <= z_slider_x_coord_max && y >= y_coord_min && y <= y_coord_max) {
        z_slider_value = (y - y_coord_min) / (y_coord_max - y_coord_min) * (slider_max - slider_min) + slider_min;
        // If over z slider
    } else if (x >= s_slider_x_coord_min && x <= s_slider_x_coord_max && y >= y_coord_min && y <= y_coord_max) {
        scale_slider_value = (y - y_coord_min) / (y_coord_max - y_coord_min) * (slider_max - slider_min) + slider_min;
    }
}


void click_callback(int button, int state, int x, int y) {
    if (button == 0 && state == 1)
        slider_move(x, y);
}


void motion_callback(int x, int y) {
    slider_move(x, y);
}


int main(int argc, char **argv) {
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1000, 1000);

    // Read in .raw file
    char filename[256] = "heart-289-240-267.raw";
    if (read_raw(filename))
        std::cout << "Read file successfully: " << filename << std::endl;
    else
        std::cout << "Failed to read file: " << filename << std::endl;

    // Start running project
    glutCreateWindow("CEG4500_Project3");
    glutMouseFunc(click_callback);
    glutMotionFunc(motion_callback);
    glutDisplayFunc(draw);
    glutIdleFunc(glutPostRedisplay);
    glutMainLoop();

    return 0;
}