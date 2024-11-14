#include <iostream>
#include <GL/glut.h>
#include <chrono>

#include "ply.h"

int num_elems;
Vertex** vlist;
Face** flist;


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


float get_max_coord_value() {
    // Gets the largest absolute value of any coordinate on any axis
    float max_coord = 0.0;
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
    return max_coord;
}


void render_faces() {
    // Draw all but last line
    glBegin(GL_LINES);
    for (int face_index = 0; face_index < num_elems; ++face_index) {
        Face* face = flist[face_index];
        for (int i = 0; i < face->nverts - 1; ++i) {
            glVertex3f(vlist[face->verts[i]]->x, vlist[face->verts[i]]->y, vlist[face->verts[i]]->z);
            glVertex3f(vlist[face->verts[i + 1]]->x, vlist[face->verts[i + 1]]->y, vlist[face->verts[i + 1]]->z);
        }
        // Draw last line
        glVertex3f(vlist[face->verts[face->nverts - 1]]->x, vlist[face->verts[face->nverts - 1]]->y, vlist[face->verts[face->nverts - 1]]->z);
        glVertex3f(vlist[face->verts[0]]->x, vlist[face->verts[0]]->y, vlist[face->verts[0]]->z);

    }
    glEnd();
}


void draw() {
    // Initialization
    glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0, 0, 0);

    // Figure out how much to scale by
    float max_coord = get_max_coord_value();
    float target_max_coord = 1.2;  // Max value we want to reach
    float scale_factor = target_max_coord / max_coord;  // How much to scale by
    std::cout << "Original size: " << max_coord << std::endl;
    std::cout << "Scale factor: " << scale_factor << std::endl;
    std::cout << "New size: " << scale_factor * max_coord << std::endl;

    // Draw side view
    gluOrtho2D(-2, 2, -2, 2);
    glScalef(scale_factor, scale_factor, scale_factor);
    glTranslatef(0.2, 0.1, 0);
    render_faces();
    glTranslatef(-0.2, -0.1, 0);

    // Draw top view
    glTranslatef(-0.15, 0.2, 0);
    glRotatef(90, 1, 0, 0);
    render_faces();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0.15, -0.2, 0);

    // Draw front view
    glTranslatef(-0.15, -0.1, 0);
    glRotatef(90, 0, 1, 0);
    render_faces();

    glFlush();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(50, 100);
	glutInitWindowSize(400, 300);
    read_ply("bunny.ply");
	glutCreateWindow("title bar");
	glutDisplayFunc(draw);
	glutMainLoop();
}
