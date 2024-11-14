#include <iostream>
#include <GL/glut.h>
#include <chrono>

#include "ply.h"

int num_elems;
Vertex** vlist;
Face** flist;

float x_slider_value = 0;
float y_slider_value = 0;
float z_slider_value = 0;

float target_max_coord = 0.4;  // Max value we want to scale up/down to

float max_coord = 0;
float min_x = 1000;
float max_x = -1000;
float min_y = 1000;
float max_y = -1000;
float min_z = 1000;
float max_z = -1000;

auto start = std::chrono::high_resolution_clock::now();


void read_ply(char* filename) {
    int i, j, k;
    PlyFile* ply;
    int nelems;
    char** elist;
    int file_type;
    float version;
    int nprops;
    PlyProperty** plist;
    char* elem_name;
    int num_comments;
    char** comments;
    int num_obj_info;
    char** obj_info;

    /* open a PLY file for reading */
    ply = ply_open_for_reading(filename, &nelems, &elist, &file_type, &version);

    /* print what we found out about the file */
    printf("version %f\n", version);
    printf("type %d\n", file_type);

    /* go through each kind of element that we learned is in the file */
    /* and read them */

    for (i = 0; i < nelems; i++) {

        /* get the description of the first element */
        elem_name = elist[i];
        plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);

        /* print the name of the element, for debugging */
        printf("element %s %d\n", elem_name, num_elems);

        /* if we're on vertex elements, read them in */
        if (equal_strings((char*)"vertex", elem_name)) {

            /* create a vertex list to hold all the vertices */
            vlist = (Vertex**)malloc(sizeof(Vertex*) * num_elems);

            /* set up for getting vertex elements */

            ply_get_property(ply, elem_name, &vert_props[0]);
            ply_get_property(ply, elem_name, &vert_props[1]);
            ply_get_property(ply, elem_name, &vert_props[2]);

            /* grab all the vertex elements */
            for (j = 0; j < num_elems; j++) {

                /* grab and element from the file */
                vlist[j] = (Vertex*)malloc(sizeof(Vertex));
                ply_get_element(ply, (void*)vlist[j]);

                /* print out vertex x,y,z for debugging */
                // printf("vertex: %g %g %g\n", vlist[j]->x, vlist[j]->y, vlist[j]->z);
            }
        }

        /* if we're on face elements, read them in */
        if (equal_strings((char*)"face", elem_name)) {

            /* create a list to hold all the face elements */
            flist = (Face**)malloc(sizeof(Face*) * num_elems);

            /* set up for getting face elements */

            ply_get_property(ply, elem_name, &face_props[0]);
            ply_get_property(ply, elem_name, &face_props[1]);

            /* grab all the face elements */
            for (j = 0; j < num_elems; j++) {

                /* grab and element from the file */
                flist[j] = (Face*)malloc(sizeof(Face));
                ply_get_element(ply, (void*)flist[j]);

                /* print out face info, for debugging */
                // printf("face: %d, list = ", flist[j]->intensity);
                // for (k = 0; k < flist[j]->nverts; k++)
                //     printf("%d ", flist[j]->verts[k]);
                // printf("\n");
            }
        }

        /* print out the properties we got, for debugging */
        for (j = 0; j < nprops; j++)
            printf("property %s\n", plist[j]->name);
    }

    /* grab and print out the comments in the file */
    comments = ply_get_comments(ply, &num_comments);
    for (i = 0; i < num_comments; i++)
        printf("comment = '%s'\n", comments[i]);

    /* grab and print out the object information */
    obj_info = ply_get_obj_info(ply, &num_obj_info);
    for (i = 0; i < num_obj_info; i++)
        printf("obj_info = '%s'\n", obj_info[i]);

    /* close the PLY file */
    ply_close(ply);
}


void get_max_coord_value() {
    // Gets the largest absolute value of any coordinate on any axis
    for (int face_index = 0; face_index < num_elems; ++face_index) {
        Face* face = flist[face_index];
        for (int i = 0; i < face->nverts; ++i) {
            if (abs(vlist[face->verts[i]]->x) > max_coord)
                max_coord = abs(vlist[face->verts[i]]->x);
            if (abs(vlist[face->verts[i]]->y) > max_coord)
                max_coord = abs(vlist[face->verts[i]]->y);
            if (abs(vlist[face->verts[i]]->z) > max_coord)
                max_coord = abs(vlist[face->verts[i]]->z);
        }
    }
}


void get_coord_boundaries() {
    // Gets the largest absolute value of any coordinate on any axis
    for (int face_index = 0; face_index < num_elems; ++face_index) {
        Face* face = flist[face_index];
        for (int i = 0; i < face->nverts; ++i) {
            if (abs(vlist[face->verts[i]]->x) < min_x)
                min_x = abs(vlist[face->verts[i]]->x);
            if (abs(vlist[face->verts[i]]->x) > max_x)
                max_x = abs(vlist[face->verts[i]]->x);

            if (abs(vlist[face->verts[i]]->y) < min_y)
                min_y = abs(vlist[face->verts[i]]->x);
            if (abs(vlist[face->verts[i]]->y) > max_y)
                max_y = abs(vlist[face->verts[i]]->y);

            if (abs(vlist[face->verts[i]]->z) < min_z)
                min_z = abs(vlist[face->verts[i]]->x);
            if (abs(vlist[face->verts[i]]->z) > max_z)
                max_z = abs(vlist[face->verts[i]]->z);
        }
    }
}


float map(float value, float old_min, float old_max, float new_min, float new_max) {
    return (value - old_min) / (old_max - old_min) * (new_max - new_min) + new_min;
}


void render_faces(float scale_factor) {
    // Draw all but last triangle
    glScalef(scale_factor, scale_factor, scale_factor);
    glBegin(GL_TRIANGLES);
    for (int face_index = 0; face_index < num_elems; ++face_index) {
        Face* face = flist[face_index];
        for (int i = 0; i < face->nverts - 2; ++i) {
            glColor3f((vlist[face->verts[i]]->x - min_x) / (max_x - min_x), (vlist[face->verts[i]]->y - min_y) / (max_y - min_y), (vlist[face->verts[i]]->z - min_z) / (max_z - min_z));
            glVertex3f(vlist[face->verts[i]]->x, vlist[face->verts[i]]->y, vlist[face->verts[i]]->z);

            glColor3f((vlist[face->verts[i + 1]]->x - min_x) / (max_x - min_x), (vlist[face->verts[i + 1]]->y - min_y) / (max_y - min_y), (vlist[face->verts[i + 1]]->z - min_z) / (max_z - min_z));
            glVertex3f(vlist[face->verts[i + 1]]->x, vlist[face->verts[i + 1]]->y, vlist[face->verts[i + 1]]->z);

            glColor3f((vlist[face->verts[i + 2]]->x - min_x) / (max_x - min_x), (vlist[face->verts[i + 2]]->y - min_y) / (max_y - min_y), (vlist[face->verts[i + 2]]->z - min_z) / (max_z - min_z));
            glVertex3f(vlist[face->verts[i + 2]]->x, vlist[face->verts[i + 2]]->y, vlist[face->verts[i + 2]]->z);
        }
        // Draw last triangle
        glColor3f((vlist[face->verts[face->nverts - 2]]->x - min_x) / (max_x - min_x), (vlist[face->verts[face->nverts - 2]]->y - min_y) / (max_y - min_y), (vlist[face->verts[face->nverts - 2]]->z - min_z) / (max_z - min_z));
        glVertex3f(vlist[face->verts[face->nverts - 2]]->x, vlist[face->verts[face->nverts - 2]]->y, vlist[face->verts[face->nverts - 2]]->z);

        glColor3f((vlist[face->verts[face->nverts - 1]]->x - min_x) / (max_x - min_x), (vlist[face->verts[face->nverts - 1]]->y - min_y) / (max_y - min_y), (vlist[face->verts[face->nverts - 1]]->z - min_z) / (max_z - min_z));
        glVertex3f(vlist[face->verts[face->nverts - 1]]->x, vlist[face->verts[face->nverts - 1]]->y, vlist[face->verts[face->nverts - 1]]->z);

        glColor3f((vlist[face->verts[0]]->x - min_x) / (max_x - min_x), (vlist[face->verts[0]]->y - min_y) / (max_y - min_y), (vlist[face->verts[0]]->z - min_z) / (max_z - min_z));
        glVertex3f(vlist[face->verts[0]]->x, vlist[face->verts[0]]->y, vlist[face->verts[0]]->z);
    }
    glEnd();
    glScalef(1 / scale_factor, 1 / scale_factor, 1 / scale_factor);
}


float slider_value_to_angle(float slider_value) {
    return map(slider_value, -1, 1, -180, 180);
}


float slider_to_coordinate(float min_coord, float max_coord, float slider_value) {
    // Map slider value of -1 to min, +1 to max
    return map(slider_value, -1, 1, min_coord, max_coord);
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
    glColor3f(1, 1, 1);

    // Scale model to fit in quadrant
    float scale_factor = target_max_coord / max_coord;  // How much to scale by

    // Get rotation offsets from sliders
    float x_angle_offset = slider_value_to_angle(x_slider_value);
    float y_angle_offset = slider_value_to_angle(y_slider_value);
    float z_angle_offset = slider_value_to_angle(z_slider_value);

    // Draw side view
    glTranslatef(0.5, 0.5, 0);
    glRotatef(x_angle_offset, 1, 0, 0);
    glRotatef(y_angle_offset, 0, 1, 0);
    glRotatef(z_angle_offset, 0, 0, 1);
    render_faces(scale_factor);
    glRotatef(-z_angle_offset, 0, 0, 1);
    glRotatef(-y_angle_offset, 0, 1, 0);
    glRotatef(-x_angle_offset, 1, 0, 0);
    glTranslatef(-0.5, -0.5, 0);

    // Draw top view
    glTranslatef(-0.5, 0.5, 0);
    glRotatef(-90 + x_angle_offset, 1, 0, 0);
    glRotatef(y_angle_offset, 0, 1, 0);
    glRotatef(z_angle_offset, 0, 0, 1);
    render_faces(scale_factor);
    glRotatef(-z_angle_offset, 0, 0, 1);
    glRotatef(-y_angle_offset, 0, 1, 0);
    glRotatef(-(-90 + x_angle_offset), 1, 0, 0);
    glTranslatef(0.5, -0.5, 0);

    // Draw front view
    glTranslatef(-0.5, -0.5, 0);
    glRotatef(x_angle_offset, 1, 0, 0);
    glRotatef(90 + y_angle_offset, 0, 1, 0);
    glRotatef(z_angle_offset, 0, 0, 1);
    render_faces(scale_factor);
    glRotatef(-z_angle_offset, 0, 0, 1);
    glRotatef(-(90 + y_angle_offset), 0, 1, 0);
    glRotatef(-x_angle_offset, 1, 0, 0);
    glTranslatef(0.5, 0.5, 0);

    // Draw back of sliders
    float slider_min_coord = -0.15;
    float slider_max_coord = -0.85;
    draw_rect(0.3, 0.35, slider_min_coord + 0.05, slider_max_coord - 0.05, 0, 0.4, 0, 0);  // X
    draw_rect(0.4, 0.45, slider_min_coord + 0.05, slider_max_coord - 0.05, 0, 0, 0.4, 0);  // Y
    draw_rect(0.5, 0.55, slider_min_coord + 0.05, slider_max_coord - 0.05, 0, 0, 0, 0.4);  // Z

    // Draw slider buttons
    float x_coord = slider_to_coordinate(slider_min_coord, slider_max_coord, x_slider_value);
    float y_coord = slider_to_coordinate(slider_min_coord, slider_max_coord, y_slider_value);
    float z_coord = slider_to_coordinate(slider_min_coord, slider_max_coord, z_slider_value);
    draw_rect(0.3, 0.35, x_coord + 0.05, x_coord - 0.05, -0.1, 1, 0, 0); // X
    draw_rect(0.4, 0.45, y_coord + 0.05, y_coord - 0.05, -0.1, 0, 1, 0); // Y
    draw_rect(0.5, 0.55, z_coord + 0.05, z_coord - 0.05, -0.1, 0, 0, 1); // Z

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
    }
}


void click_callback(int button, int state, int x, int y) {
    if (button == 0 && state == 1)
        slider_move(x, y);
}


void motion_callback(int x, int y) {
    slider_move(x, y);
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1000, 1000);
    read_ply("bunny.ply");
    get_max_coord_value();
    get_coord_boundaries();
	glutCreateWindow("CEG4500_Project2");
    glutMouseFunc(click_callback);
    glutMotionFunc(motion_callback);
	glutDisplayFunc(draw);
    glutIdleFunc(glutPostRedisplay);
	glutMainLoop();
}
