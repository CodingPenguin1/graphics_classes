#include <iostream>
#include <GL/glut.h>
#include <chrono>

void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}

void display() {
    // Framerate counter
    static double last_time = 0;
    static int num_frames = 0;
    static int current_fps = 0;

    double time_now = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();

    ++num_frames;

    if (time_now - last_time > 1) {
        last_time = time_now;
        current_fps = num_frames;
        num_frames = 0;
        std::cout << current_fps << std::endl;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2i(180, 15);
    glVertex2i(10, 145);
    glEnd();

    glFlush();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(50, 100);
    glutInitWindowSize(400, 300);
    glutCreateWindow("title bar");
    init();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMainLoop();
}