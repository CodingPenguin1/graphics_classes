/*
 * Solution for project 0 of the course CEG476/676 taught by Thomas
 * Wischgoll.
 *
 * This solution kept simple and can be used as starting point for the
 * following projects. It creates a top, side, and front view of the
 * geometry retreived from a ply file using Greg Turk's ply reader.
 *
 * Thomas Wischgoll, October 2007.
 */

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "ply.h"

#include <iostream>

using std::cout;
using std::endl;

#include <math.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <chrono>
#include <string.h>


#define CROSSPROD(p1, p2, p3)         \
    p3.x = p1.y * p2.z - p1.z * p2.y; \
    p3.y = p1.z * p2.x - p1.x * p2.z; \
    p3.z = p1.x * p2.y - p1.y * p2.x

#define EPS 0.00001

#define DTOR 0.0174532925
#define RTOD 57.2957795
#define TWOPI 6.283185307179586476925287
#define PI 3.141592653589793238462643

typedef struct {
    GLdouble x, y, z;
} recVec;

typedef struct {
    GLdouble r, g, b;
} recColor;

typedef struct {
    recVec viewPos;                   // View position
    recVec viewDir;                   // View direction vector
    recVec viewUp;                    // View up direction
    recVec rotPoint;                  // Point to rotate about
    GLdouble focalLength;             // Focal Length along view direction
    GLdouble aperture;                // gCamera aperture
    GLdouble eyeSep;                  // Eye separation
    GLint screenWidth, screenHeight;  // current window/screen height and width
} recCamera;

typedef struct {
    float x;
    float y;
    float z;
} point;

int width, height;

// ply vertices and faces
Vertex **vlist;
Face **flist;
int num_elems;

GLuint gColorScheme = 1;
GLuint gSubDivisions = 64;
GLuint gIJRatio = 3;
recVec *gVertexPos = NULL, *gVertexNormal = NULL;
recColor *gVertexColor = NULL;
GLuint gPolyList = 0;

char gSurfName[256] = "";
char gSurfCredit[256] = "";
char gSurfX[256] = "";
char gSurfY[256] = "";
char gSurfZ[256] = "";
char gSurfRange[256] = "";

int main_window = 0;

GLint gDollyPanStartPoint[2] = {0, 0};
GLfloat gTrackBallRotation[4] = {0.0, 0.0, 0.0, 0.0};
GLboolean gDolly = GL_FALSE;
GLboolean gPan = GL_FALSE;
GLboolean gTrackBall = GL_FALSE;
GLfloat gWorldRotation[4] = {100.0, -0.7, 0.6, 0.5};

GLboolean gPoints = GL_FALSE;
GLboolean gLines = GL_FALSE;
GLboolean gPolygons = GL_TRUE;
GLboolean gShowHelp = GL_TRUE;
GLboolean gShowInfo = GL_TRUE;
GLboolean gShowCredits = GL_TRUE;
GLboolean gLighting = 4;
GLint gSurface = 0;

GLdouble gShapeSize = 11.0;
recCamera gCamera;
recVec gOrigin = {0.0, 0.0, 0.0};

GLboolean gDragStart = false;

int gMainWindow = 0;

#define animation_duration 10
double start_time;


void computeNormals() {
    int j, k;
    double normal[3], v1[3], v2[3], length;

    if (fabs(vlist[flist[0]->verts[0]]->nx +
                vlist[flist[0]->verts[0]]->ny +
                vlist[flist[0]->verts[0]]->nz) > 0.05) {
#ifdef DEBUG
        printf("normal information present; do not need to calculate\n");
#endif
        return;
    }

    for (j = 0; j < num_elems; j++) {
        // compute normal for face first
        v1[0] = vlist[flist[j]->verts[0]]->x - vlist[flist[j]->verts[1]]->x;
        v1[1] = vlist[flist[j]->verts[0]]->y - vlist[flist[j]->verts[1]]->y;
        v1[2] = vlist[flist[j]->verts[0]]->z - vlist[flist[j]->verts[1]]->z;
        v2[0] = vlist[flist[j]->verts[0]]->x - vlist[flist[j]->verts[2]]->x;
        v2[1] = vlist[flist[j]->verts[0]]->y - vlist[flist[j]->verts[2]]->y;
        v2[2] = vlist[flist[j]->verts[0]]->z - vlist[flist[j]->verts[2]]->z;
        normal[0] = v1[1] * v2[2] - v1[2] * v2[1];
        normal[1] = v1[2] * v2[0] - v1[0] * v2[2];
        normal[2] = v1[0] * v2[1] - v1[1] * v2[0];

#ifdef DEBUG
        printf("V1 vector: (%lf,%lf,%lf)\n",
                v1[0], v1[1], v1[2]);
        printf("V2 vector: (%lf,%lf,%lf)\n",
                v2[0], v2[1], v2[2]);
        printf("Normal vector: (%lf,%lf,%lf)\n",
                normal[0], normal[1], normal[2]);
#endif

        // update normal vector for vertices
        for (k = 0; k < flist[j]->nverts; k++) {
            vlist[flist[j]->verts[k]]->nx += normal[0];
            vlist[flist[j]->verts[k]]->ny += normal[1];
            vlist[flist[j]->verts[k]]->nz += normal[2];
        }
    }

    // normalize normals
    for (j = 0; j < num_elems; j++) {
        for (k = 0; k < flist[j]->nverts; k++) {
            length = sqrt(vlist[flist[j]->verts[k]]->nx *
                            vlist[flist[j]->verts[k]]->nx +
                    vlist[flist[j]->verts[k]]->ny *
                            vlist[flist[j]->verts[k]]->ny +
                    vlist[flist[j]->verts[k]]->nz *
                            vlist[flist[j]->verts[k]]->nz);
            vlist[flist[j]->verts[k]]->nx /= length;
            vlist[flist[j]->verts[k]]->ny /= length;
            vlist[flist[j]->verts[k]]->nz /= length;
        }
    }
}


void draw() {
    // draw the entire geometry by looping through the faces which in
    // turn reference the vertices
    for (int j = 0; j < num_elems; j++) {
        glBegin(GL_POLYGON);
        for (int k = 0; k < flist[j]->nverts; k++) {
            glNormal3f(vlist[flist[j]->verts[k]]->nx,
                    vlist[flist[j]->verts[k]]->ny,
                    vlist[flist[j]->verts[k]]->nz);
            glVertex3f(vlist[flist[j]->verts[k]]->x,
                    vlist[flist[j]->verts[k]]->y,
                    vlist[flist[j]->verts[k]]->z);
        }
        glEnd();
    }
}


void gCameraReset(void) {
    gCamera.aperture = 50;
    gCamera.focalLength = 9;
    gCamera.eyeSep = gCamera.focalLength / 20;
    gCamera.rotPoint = gOrigin;

    gCamera.viewPos.x = 0.0;
    gCamera.viewPos.y = 0.0;
    gCamera.viewPos.z = -gCamera.focalLength;
    gCamera.viewDir.x = -gCamera.viewPos.x;
    gCamera.viewDir.y = -gCamera.viewPos.y;
    gCamera.viewDir.z = -gCamera.viewPos.z;

    gCamera.viewUp.x = 0;
    gCamera.viewUp.y = 1;
    gCamera.viewUp.z = 0;
}


void normalise(recVec *p) {
    double length;

    length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
    if (length != 0) {
        p->x /= length;
        p->y /= length;
        p->z /= length;
    } else {
        p->x = 0;
        p->y = 0;
        p->z = 0;
    }
}


void SetLighting(void) {
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {90.0};

    GLfloat position[4] = {7.0, -7.0, 12.0, 0.0};
    GLfloat ambient[4] = {0.2, 0.2, 0.2, 1.0};
    GLfloat diffuse[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat specular[4] = {1.0, 1.0, 1.0, 1.0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    switch (gLighting) {
        case 0:
            break;
        case 1:
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
            glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
            break;
        case 2:
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
            glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
            break;
        case 3:
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
            glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
            break;
        case 4:
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
            glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
            break;
    }

    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glEnable(GL_LIGHT0);
}


void BuildGeometry(void) {
    if (gPolyList)
        glDeleteLists(gPolyList, 1);
    gPolyList = glGenLists(1);
    glNewList(gPolyList, GL_COMPILE);
    draw();
    glEndList();
}


void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    gCamera.screenWidth = w;
    gCamera.screenHeight = h;

#ifdef EMULATE_STEREO
    GLubyte *bitmap = new GLubyte[gCamera.screenWidth *
            gCamera.screenHeight / 8];
    for (int i = 0; i < gCamera.screenWidth *
                    gCamera.screenHeight / 8;
            i++)
        bitmap[i] = 85;

    glClearStencil(0x0);
    glDrawPixels(gCamera.screenWidth,
            gCamera.screenHeight,
            GL_STENCIL_INDEX,
            GL_BITMAP,
            bitmap);
    delete bitmap;
#endif

    glutPostRedisplay();
}


void display(void) {
    GLdouble ratio, radians, wd2, ndfl;
    GLdouble left, right, top, bottom, nearc, farc;

    nearc = -gCamera.viewPos.z - gShapeSize * 0.5;
    if (nearc < 0.1) nearc = 0.1;
    farc = -gCamera.viewPos.z + gShapeSize * 0.5;

    // Misc stuff
    ratio = gCamera.screenWidth / (double)gCamera.screenHeight;
    radians = DTOR * gCamera.aperture / 2;
    wd2 = nearc * tan(radians);
    ndfl = nearc / gCamera.focalLength;

    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    left = -ratio * wd2;
    right = ratio * wd2;
    top = wd2;
    bottom = -wd2;
    glFrustum(left, right, bottom, top, nearc, farc);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z,
            0.0, 0.0, 0.0,
            gCamera.viewUp.x, gCamera.viewUp.y, gCamera.viewUp.z);

    glEnable(GL_LIGHTING);
    glCallList(gPolyList);
    glDisable(GL_LIGHTING);

    glutSwapBuffers();
}


void interpolate(point &interpolated_point, const point &a, const point &b, const float t) {
    interpolated_point.x = a.x + (t * (b.x - a.x));
    interpolated_point.y = a.y + (t * (b.y - a.y));
    interpolated_point.z = a.z + (t * (b.z - a.z));
}


void bezier(point &interpolated_point, const point &a, const point &b, const point &c, const point &d, const point &e, const point &f, const float t) {
    // Points from interpolated lines
    point ab, bc, cd, de, ef;
    point abbc, bccd, cdde, deef;
    point abbcbccd, bccdcdde, cddedeef;
    point abbcbccdcdde, bccdcddedeef;  // I know this is terrible but it works so don't @ me

    // Interpolate first layer of lines/points
    interpolate(ab, a, b, t);
    interpolate(bc, b, c, t);
    interpolate(cd, c, d, t);
    interpolate(de, d, e, t);
    interpolate(ef, e, f, t);

    // Interpolate second layer of lines/points
    interpolate(abbc, ab, bc, t);
    interpolate(bccd, bc, cd, t);
    interpolate(cdde, cd, de, t);
    interpolate(deef, de, ef, t);

    //Interpolate third layer of lines/points
    interpolate(abbcbccd, abbc, bccd, t);
    interpolate(bccdcdde, bccd, cdde, t);
    interpolate(cddedeef, cdde, deef, t);

    // Interpolate 4th layer of lines/points
    interpolate(abbcbccdcdde, abbcbccd, bccdcdde, t);
    interpolate(bccdcddedeef, bccdcdde, cddedeef, t);

    // Interpolate final point
    interpolate(interpolated_point, abbcbccdcdde, bccdcddedeef, t);
}


void animate(void) {
    // Control points
    const point a = {-0.7, 0.5, 0.0};
    const point b = {-0.7, 0.7, 1.0};
    const point c = {0.0, 0.3, 1.0};
    const point d = {0.0, 1.0, 0.0};
    const point e = {0.0, 1.0, -1.0};
    const point f = {-0.7, 0.5, 0.0};

    // Calculate t
    double time_now = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();
    float t = ((float)(time_now - start_time)) / animation_duration;
    while (t > 1)
        t--;

    // Calculate new camera location
    point new_location;
    bezier(new_location, a, b, c, d, e, f, t);
    std::cout << t << ":\t\t" << new_location.x << "\t\t" << new_location.y << "\t\t" << new_location.z << std::endl;

    // Move camera
    gCamera.viewPos.x = new_location.x;
    gCamera.viewPos.y = new_location.y;
    gCamera.viewPos.z = new_location.z;

    glutPostRedisplay();
}


void read_ply(char *filename) {
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

    /* open a PLY file for reading */
    ply = ply_open_for_reading(filename, &nelems, &elist, &file_type, &version);

    /* go through each kind of element that we learned is in the file and read them*/
    for (i = 0; i < nelems; i++) {

        /* get the description of the first element */
        elem_name = elist[i];
        plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);

        /* if we're on vertex elements, read them in */
        if (equal_strings("vertex", elem_name)) {

            /* create a vertex list to hold all the vertices */
            vlist = (Vertex **)malloc(sizeof(Vertex *) * num_elems);

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
                vlist[j] = (Vertex *)malloc(sizeof(Vertex));
                ply_get_element(ply, (void *)vlist[j]);
            }
        }

        /* if we're on face elements, read them in */
        if (equal_strings("face", elem_name)) {

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

    /* grab and print out the comments in the file */
    comments = ply_get_comments(ply, &num_comments);

    /* grab and print out the object information */
    obj_info = ply_get_obj_info(ply, &num_obj_info);

    /* close the PLY file */
    ply_close(ply);

    computeNormals();
}


int main(int argc, char **argv) {
    // Set start time of program
    start_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << start_time << std::endl;

    // Read the ply file
    if (argc == 2) {
        read_ply(argv[1]);
    } else {
        read_ply("../ply_files/bunny.ply");
    }

    // Set up window
    glutInit(&argc, (char **)argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  // non-stereo for main window
    glutInitWindowSize(800, 600);
    gMainWindow = glutCreateWindow("[floating] CEG 4510 Project 1");

    // Lighting and rendering initialization
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DITHER);
    glDisable(GL_CULL_FACE);
    glLineWidth(1.0);
    glPointSize(1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glFrontFace(GL_CCW);
    glClearColor(0.0, 0.0, 0.0, 0.0); /* Background recColor */
    gCameraReset();
    SetLighting();
    BuildGeometry();

    // Rendering functions
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(animate);

    glutMainLoop();
    return 0;
}
