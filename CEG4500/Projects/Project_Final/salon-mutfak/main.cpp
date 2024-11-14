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

//#define USE_JOYSTICK
//#define USE_WII

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <iostream>

using std::cout;
using std::endl;

#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>
//#include <GL/glext.h>
#include <GL/gl.h>

#include "Point3D.h"
#include "salon-mutfak.h"
#include "trackball.h"


using namespace km::Drawing;

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

int width, height;

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
GLfloat gTrackBallRotation[4] = {0.0, 0.0, 1.0, 0.0};
GLboolean gDolly = GL_FALSE;
GLboolean gPan = GL_FALSE;
GLboolean gTrackBall = GL_FALSE;
GLfloat gWorldRotation[4] = {180.0, 0.0, 1.0, 0.0};
GLfloat gWorldTranslation[3] = {0.0, 0.0, 0.0};

GLboolean gStereo = GL_FALSE;
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

int gLastKey = ' ';

int gMainWindow = 0;

int button = -1, state = -1;
double jsign;
#ifdef USE_JOYSTICK
#include "BasicJoystick.h"
BasicJoystick js;
#endif
#ifdef USE_WII
#include "Wii.h"
Wii wii;
#endif


// #### ADDED FOR SHADING ####
#define RENDER_WIDTH 1920
#define RENDER_HEIGHT 1080
#define SHADOW_MAP_RATIO 2

//Light position
float p_light[4] = {7, 70, 0, 0};
// float p_light[4] = {252.589f, 105.177f, 272.436f};

//Light lookAt
float l_light[3] = {0, 60, 0};
// float l_light[3] = {252.589f, 105.177f, 172.436f};

// Hold id of the framebuffer for light POV rendering
GLuint fboId, depth_rb;

// Z values will be rendered to this texture when using fboId framebuffer
GLuint depthTextureId;

// Use to activate/disable shadowShader
GLhandleARB shadowShaderId;
GLuint shadowMapUniform;

// ###########################


void draw() {
    Cube();
    mesh1();
    mesh10();
    mesh100_001_0();
    mesh101_001_0();
    mesh102();
    mesh103();
    mesh104();
    mesh105();
    mesh106();
    mesh107();
    mesh108();
    mesh109();
    mesh11();
    mesh110();
    mesh111();
    mesh112();
    mesh113();
    mesh114();
    mesh115();
    mesh116();
    mesh117();
    mesh118();
    mesh119();
    mesh12();
    mesh120();
    mesh121();
    mesh122_001_0();
    mesh122_002_0();
    mesh122_003_0();
    mesh122_004_0();
    mesh123_001_0();
    mesh123_002_0();
    mesh128_002_0();
    mesh13();
    mesh131();
    mesh132();
    mesh133();
    mesh134();
    mesh135();
    mesh136();
    mesh137();
    mesh138();
    mesh139();
    mesh14();
    mesh140();
    mesh141();
    mesh142();
    mesh143();
    mesh144();
    mesh145();
    mesh146();
    mesh147();
    mesh148();
    mesh149();
    mesh15_001_0();
    mesh150();
    mesh151();
    mesh152_001_0();
    mesh152_002_0();
    mesh152_003_0();
    mesh152_004_0();
    mesh152_005_0();
    mesh152_006_0();
    mesh16();
    mesh168_001_0();
    mesh17();
    mesh18();
    mesh188_001_0();
    mesh188_002_0();
    mesh188_003_0();
    mesh188_004_0();
    mesh188_005_0();
    mesh189();
    mesh19_001_0();
    mesh19_002_0();
    mesh19_003_0();
    mesh19_004_0();
    mesh19_005_0();
    mesh19_006_0();
    mesh19_007_0();
    mesh19_008_0();
    mesh19_009_0();
    mesh19_010_0();
    mesh19_011_0();
    mesh19_012_0();
    mesh190();
    mesh191_001_0();
    mesh191_002_0();
    mesh191_003_0();
    mesh191_004_0();
    mesh192();
    mesh193_001_0();
    mesh194_001_0();
    mesh195_001_0();
    mesh197_001_0();
    mesh198_001_0();
    mesh199_001_0();
    mesh2();
    mesh200_001_0();
    mesh201_001_0();
    mesh202_001_0();
    mesh203_001_0();
    mesh205_001_0();
    mesh206_001_0();
    mesh206_002_0();
    mesh207_001_0();
    mesh207_002_0();
    mesh208_001_0();
    mesh208_002_0();
    mesh209_001_0();
    mesh209_002_0();
    mesh21_001_0();
    mesh21_002_0();
    mesh210_001_0();
    mesh211_001_0();
    mesh212_001_0();
    mesh213_001_0();
    mesh214_001_0();
    mesh215_001_0();
    mesh216_001_0();
    mesh217_001_0();
    mesh218_001_0();
    mesh219_001_0();
    mesh22_001_0();
    mesh22_002_0();
    mesh22_003_0();
    mesh22_004_0();
    mesh220_001_0();
    mesh221_001_0();
    mesh222_001_0();
    mesh223_001_0();
    mesh224_001_0();
    mesh225_001_0();
    mesh226_001_0();
    mesh227_001_0();
    mesh228_001_0();
    mesh229_001_0();
    mesh23();
    mesh230();
    mesh231();
    mesh232_001_0();
    mesh232_002_0();
    mesh232_003_0();
    mesh232_004_0();
    mesh232_005_0();
    mesh233();
    mesh234();
    mesh235();
    mesh236();
    mesh237();
    mesh238();
    mesh239();
    mesh24();
    mesh240();
    mesh241();
    // mesh242();
    // mesh243();
    mesh244();
    mesh245();
    mesh246();
    mesh247();
    mesh248();
    mesh249();
    mesh25();
    mesh250();
    mesh251();
    mesh252();
    mesh253();
    mesh254();
    mesh255();
    mesh256();
    mesh257();
    mesh258();
    mesh259();
    mesh26();
    mesh260();
    mesh261();
    mesh262();
    mesh263();
    mesh264();
    mesh265();
    mesh266();
    mesh267();
    mesh268();
    mesh27();
    mesh28_001_0();
    mesh3();
    mesh30_001_0();
    mesh31_001_0();
    mesh34_001_0();
    mesh35_001_0();
    mesh35_002_0();
    mesh35_003_0();
    mesh35_004_0();
    mesh38_001_0();
    mesh39_001_0();
    mesh4();
    mesh40_001_0();
    mesh41_001_0();
    mesh42_001_0();
    mesh43_001_0();
    mesh44_001_0();
    mesh45_001_0();
    mesh46_001_0();
    mesh47_001_0();
    mesh48_001_0();
    mesh49_001_0();
    mesh5();
    mesh50_001_0();
    mesh51_001_0();
    mesh52_001_0();
    mesh53_001_0();
    mesh54_001_0();
    mesh55_001_0();
    mesh56_001_0();
    mesh57_001_0();
    mesh58_001_0();
    mesh58_002_0();
    mesh58_003_0();
    mesh59_001_0();
    mesh59_002_0();
    mesh59_003_0();
    mesh6();
    mesh60_001_0();
    mesh60_002_0();
    mesh60_003_0();
    mesh61_001_0();
    mesh61_002_0();
    mesh61_003_0();
    mesh63_001_0();
    mesh65_001_0();
    mesh66_001_0();
    mesh68_001_0();
    mesh7();
    mesh71_001_0();
    mesh72_001_0();
    mesh73_001_0();
    mesh8();
    mesh81_001_0();
    mesh81_002_0();
    mesh81_003_0();
    mesh81_004_0();
    mesh81_005_0();
    mesh81_006_0();
    mesh82_001_0();
    mesh82_002_0();
    mesh82_003_0();
    mesh82_004_0();
    mesh82_005_0();
    mesh82_006_0();
    mesh83_001_0();
    mesh83_002_0();
    mesh83_003_0();
    mesh83_004_0();
    mesh83_005_0();
    mesh83_006_0();
    mesh89();
    mesh9();
    mesh90();
    mesh91();
    mesh92();
    mesh93();
    mesh94();
    mesh95();
    mesh98_001_0();
    mesh99_001_0();
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

void DrawString(GLfloat x, GLfloat y, char *string) {
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, string[i]);
    }
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

recVec CalcNormal(recVec p, recVec p1, recVec p2) {
    recVec n, pa, pb;

    pa.x = p1.x - p.x;
    pa.y = p1.y - p.y;
    pa.z = p1.z - p.z;
    pb.x = p2.x - p.x;
    pb.y = p2.y - p.y;
    pb.z = p2.z - p.z;
    normalise(&pa);
    normalise(&pb);

    n.x = pa.y * pb.z - pa.z * pb.y;
    n.y = pa.z * pb.x - pa.x * pb.z;
    n.z = pa.x * pb.y - pa.y * pb.x;
    normalise(&n);

    return (n);
}

/* Based on GetColour by Paul Bourke */

recColor GetColor(double v, double vmin, double vmax, int type) {
    double dv, vmid;
    recColor c = {1.0, 1.0, 1.0};
    recColor c1, c2, c3;
    double ratio;

    if (v < vmin)
        v = vmin;
    if (v > vmax)
        v = vmax;
    dv = vmax - vmin;

    switch (type) {
        case 1:
            if (v < (vmin + 0.25 * dv)) {
                c.r = 0;
                c.g = 4 * (v - vmin) / dv;
                c.b = 1;
            } else if (v < (vmin + 0.5 * dv)) {
                c.r = 0;
                c.g = 1;
                c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
            } else if (v < (vmin + 0.75 * dv)) {
                c.r = 4 * (v - vmin - 0.5 * dv) / dv;
                c.g = 1;
                c.b = 0;
            } else {
                c.r = 1;
                c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
                c.b = 0;
            }
            break;
        case 2:
            c.r = (v - vmin) / dv;
            c.g = 0;
            c.b = (vmax - v) / dv;
            break;
        case 3:
            c.r = (v - vmin) / dv;
            c.b = c.r;
            c.g = c.r;
            break;
        case 4:
            if (v < (vmin + dv / 6.0)) {
                c.r = 1;
                c.g = 6 * (v - vmin) / dv;
                c.b = 0;
            } else if (v < (vmin + 2.0 * dv / 6.0)) {
                c.r = 1 + 6 * (vmin + dv / 6.0 - v) / dv;
                c.g = 1;
                c.b = 0;
            } else if (v < (vmin + 3.0 * dv / 6.0)) {
                c.r = 0;
                c.g = 1;
                c.b = 6 * (v - vmin - 2.0 * dv / 6.0) / dv;
            } else if (v < (vmin + 4.0 * dv / 6.0)) {
                c.r = 0;
                c.g = 1 + 6 * (vmin + 3.0 * dv / 6.0 - v) / dv;
                c.b = 1;
            } else if (v < (vmin + 5.0 * dv / 6.0)) {
                c.r = 6 * (v - vmin - 4.0 * dv / 6.0) / dv;
                c.g = 0;
                c.b = 1;
            } else {
                c.r = 1;
                c.g = 0;
                c.b = 1 + 6 * (vmin + 5.0 * dv / 6.0 - v) / dv;
            }
            break;
        case 5:
            c.r = (v - vmin) / (vmax - vmin);
            c.g = 1;
            c.b = 0;
            break;
        case 6:
            c.r = (v - vmin) / (vmax - vmin);
            c.g = (vmax - v) / (vmax - vmin);
            c.b = c.r;
            break;
        case 7:
            if (v < (vmin + 0.25 * dv)) {
                c.r = 0;
                c.g = 4 * (v - vmin) / dv;
                c.b = 1 - c.g;
            } else if (v < (vmin + 0.5 * dv)) {
                c.r = 4 * (v - vmin - 0.25 * dv) / dv;
                c.g = 1 - c.r;
                c.b = 0;
            } else if (v < (vmin + 0.75 * dv)) {
                c.g = 4 * (v - vmin - 0.5 * dv) / dv;
                c.r = 1 - c.g;
                c.b = 0;
            } else {
                c.r = 0;
                c.b = 4 * (v - vmin - 0.75 * dv) / dv;
                c.g = 1 - c.b;
            }
            break;
        case 8:
            if (v < (vmin + 0.5 * dv)) {
                c.r = 2 * (v - vmin) / dv;
                c.g = c.r;
                c.b = c.r;
            } else {
                c.r = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
                c.g = c.r;
                c.b = c.r;
            }
            break;
        case 9:
            if (v < (vmin + dv / 3)) {
                c.b = 3 * (v - vmin) / dv;
                c.g = 0;
                c.r = 1 - c.b;
            } else if (v < (vmin + 2 * dv / 3)) {
                c.r = 0;
                c.g = 3 * (v - vmin - dv / 3) / dv;
                c.b = 1;
            } else {
                c.r = 3 * (v - vmin - 2 * dv / 3) / dv;
                c.g = 1 - c.r;
                c.b = 1;
            }
            break;
        case 10:
            if (v < (vmin + 0.2 * dv)) {
                c.r = 0;
                c.g = 5 * (v - vmin) / dv;
                c.b = 1;
            } else if (v < (vmin + 0.4 * dv)) {
                c.r = 0;
                c.g = 1;
                c.b = 1 + 5 * (vmin + 0.2 * dv - v) / dv;
            } else if (v < (vmin + 0.6 * dv)) {
                c.r = 5 * (v - vmin - 0.4 * dv) / dv;
                c.g = 1;
                c.b = 0;
            } else if (v < (vmin + 0.8 * dv)) {
                c.r = 1;
                c.g = 1 - 5 * (v - vmin - 0.6 * dv) / dv;
                c.b = 0;
            } else {
                c.r = 1;
                c.g = 5 * (v - vmin - 0.8 * dv) / dv;
                c.b = 5 * (v - vmin - 0.8 * dv) / dv;
            }
            break;
        case 11:
            c1.r = 200 / 255.0;
            c1.g = 60 / 255.0;
            c1.b = 0 / 255.0;
            c2.r = 250 / 255.0;
            c2.g = 160 / 255.0;
            c2.b = 110 / 255.0;
            c.r = (c2.r - c1.r) * (v - vmin) / dv + c1.r;
            c.g = (c2.g - c1.g) * (v - vmin) / dv + c1.g;
            c.b = (c2.b - c1.b) * (v - vmin) / dv + c1.b;
            break;
        case 12:
            c1.r = 55 / 255.0;
            c1.g = 55 / 255.0;
            c1.b = 45 / 255.0;
            //      c2.r = 200 / 255.0; c2.g =  60 / 255.0; c2.b =   0 / 255.0;
            c2.r = 235 / 255.0;
            c2.g = 90 / 255.0;
            c2.b = 30 / 255.0;
            c3.r = 250 / 255.0;
            c3.g = 160 / 255.0;
            c3.b = 110 / 255.0;
            ratio = 0.4;
            vmid = vmin + ratio * dv;
            if (v < vmid) {
                c.r = (c2.r - c1.r) * (v - vmin) / (ratio * dv) + c1.r;
                c.g = (c2.g - c1.g) * (v - vmin) / (ratio * dv) + c1.g;
                c.b = (c2.b - c1.b) * (v - vmin) / (ratio * dv) + c1.b;
            } else {
                c.r = (c3.r - c2.r) * (v - vmid) / ((1 - ratio) * dv) + c2.r;
                c.g = (c3.g - c2.g) * (v - vmid) / ((1 - ratio) * dv) + c2.g;
                c.b = (c3.b - c2.b) * (v - vmid) / ((1 - ratio) * dv) + c2.b;
            }
            break;
        case 13:
            c1.r = 0 / 255.0;
            c1.g = 255 / 255.0;
            c1.b = 0 / 255.0;
            c2.r = 255 / 255.0;
            c2.g = 150 / 255.0;
            c2.b = 0 / 255.0;
            c3.r = 255 / 255.0;
            c3.g = 250 / 255.0;
            c3.b = 240 / 255.0;
            ratio = 0.3;
            vmid = vmin + ratio * dv;
            if (v < vmid) {
                c.r = (c2.r - c1.r) * (v - vmin) / (ratio * dv) + c1.r;
                c.g = (c2.g - c1.g) * (v - vmin) / (ratio * dv) + c1.g;
                c.b = (c2.b - c1.b) * (v - vmin) / (ratio * dv) + c1.b;
            } else {
                c.r = (c3.r - c2.r) * (v - vmid) / ((1 - ratio) * dv) + c2.r;
                c.g = (c3.g - c2.g) * (v - vmid) / ((1 - ratio) * dv) + c2.g;
                c.b = (c3.b - c2.b) * (v - vmid) / ((1 - ratio) * dv) + c2.b;
            }
            break;
        case 14:
            c.r = 1;
            c.g = 1 - (v - vmin) / dv;
            c.b = 0;
            break;
        case 15:
            if (v < (vmin + 0.25 * dv)) {
                c.r = 0;
                c.g = 4 * (v - vmin) / dv;
                c.b = 1;
            } else if (v < (vmin + 0.5 * dv)) {
                c.r = 0;
                c.g = 1;
                c.b = 1 - 4 * (v - vmin - 0.25 * dv) / dv;
            } else if (v < (vmin + 0.75 * dv)) {
                c.r = 4 * (v - vmin - 0.5 * dv) / dv;
                c.g = 1;
                c.b = 0;
            } else {
                c.r = 1;
                c.g = 1;
                c.b = 4 * (v - vmin - 0.75 * dv) / dv;
            }
            break;
        case 16:
            if (v < (vmin + 0.5 * dv)) {
                c.r = 0.0;
                c.g = 2 * (v - vmin) / dv;
                c.b = 1 - 2 * (v - vmin) / dv;
            } else {
                c.r = 2 * (v - vmin - 0.5 * dv) / dv;
                c.g = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
                c.b = 0.0;
            }
            break;
        case 17:
            if (v < (vmin + 0.5 * dv)) {
                c.r = 1.0;
                c.g = 1 - 2 * (v - vmin) / dv;
                c.b = 2 * (v - vmin) / dv;
            } else {
                c.r = 1 - 2 * (v - vmin - 0.5 * dv) / dv;
                c.g = 2 * (v - vmin - 0.5 * dv) / dv;
                c.b = 1.0;
            }
            break;
        case 18:
            c.r = 0;
            c.g = (v - vmin) / (vmax - vmin);
            c.b = 1;
            break;
        case 19:
            c.r = (v - vmin) / (vmax - vmin);
            c.g = c.r;
            c.b = 1;
            break;
        case 20:
            c1.r = 0 / 255.0;
            c1.g = 160 / 255.0;
            c1.b = 0 / 255.0;
            c2.r = 180 / 255.0;
            c2.g = 220 / 255.0;
            c2.b = 0 / 255.0;
            c3.r = 250 / 255.0;
            c3.g = 220 / 255.0;
            c3.b = 170 / 255.0;
            ratio = 0.3;
            vmid = vmin + ratio * dv;
            if (v < vmid) {
                c.r = (c2.r - c1.r) * (v - vmin) / (ratio * dv) + c1.r;
                c.g = (c2.g - c1.g) * (v - vmin) / (ratio * dv) + c1.g;
                c.b = (c2.b - c1.b) * (v - vmin) / (ratio * dv) + c1.b;
            } else {
                c.r = (c3.r - c2.r) * (v - vmid) / ((1 - ratio) * dv) + c2.r;
                c.g = (c3.g - c2.g) * (v - vmid) / ((1 - ratio) * dv) + c2.g;
                c.b = (c3.b - c2.b) * (v - vmid) / ((1 - ratio) * dv) + c2.b;
            }
            break;
    }
    return (c);
}

void SetLighting(void) {
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {90.0};

    // GLfloat position[4] = {7.0, -7.0, 12.0, 0.0};
    GLfloat position[4] = {p_light[0], p_light[1], p_light[2], p_light[3]};
    GLfloat ambient[4] = {0.2, 0.2, 0.2, 1.0};
    GLfloat diffuse[4] = {0.5, 0.5, 0.5, 1.0};
    GLfloat specular[4] = {0.3, 0.3, 0.3, 1.0};

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


/* Code based on work by Paul Bourke */

#define kSurfaces 5

// expects u & v (-PI to PI)
recVec Eval(double u, double v, int type) {
    recVec p;
    double temp;

    switch (type) {
        case 0:
            sprintf(gSurfName, "Tranguloid Trefoil");
            sprintf(gSurfCredit, "by Roger Bagula");
            sprintf(gSurfX, "x = 2 sin(3 u) / (2 + cos(v)) )");
            sprintf(gSurfY, "y = 2 (sin(u) + 2 sin(2 u)) / (2 + cos(v + 2 pi / 3)) ");
            sprintf(gSurfZ, "z = (cos(u) - 2 cos(2 u)) (2 + cos(v)) (2 + cos(v + 2 pi / 3)) / 4");
            sprintf(gSurfRange, "-pi <= u <= pi, -pi <= v <= pi ");
            p.x = sin(3 * u) * 2 / (2 + cos(v));
            p.y = (sin(u) + 2 * sin(2 * u)) * 2 / (2 + cos(v + TWOPI / 3));
            p.z = (cos(u) - 2 * cos(2 * u)) * (2 + cos(v)) * (2 + cos(v + TWOPI / 3)) / 4;
            break;
        case 1:
            sprintf(gSurfName, "Triaxial Tritorus");
            sprintf(gSurfCredit, "by Roger Bagula");
            sprintf(gSurfX, "x = sin(u) (1 + cos(v))");
            sprintf(gSurfY, "y = sin(u + 2pi / 3) (1 + cos(v + 2pi / 3)) ");
            sprintf(gSurfZ, "z = sin(u + 4pi / 3) (1 + cos(v + 4pi / 3))");
            sprintf(gSurfRange, "0 <= u <= 2 pi, 0 <= v <= 2 pi");
            p.x = sin(u) * (1 + cos(v));
            p.y = sin(u + 2 * PI / 3) * (1 + cos(v + 2 * PI / 3));
            p.z = sin(u + 4 * PI / 3) * (1 + cos(v + 4 * PI / 3));
            break;
        case 2:
            sprintf(gSurfName, "Stiletto Surface");
            sprintf(gSurfCredit, "by Roger Bagula");
            sprintf(gSurfX, "x =  (2 + cos(u)) cos(v)^3 sin(v)");
            sprintf(gSurfY, "y =  (2 + cos(u + 2pi /3)) cos(v + 2pi / 3)^2 sin(v + 2pi / 3)^2");
            sprintf(gSurfZ, "z = -(2 + cos(u - 2pi / 3)) cos(v + 2pi / 3)^2 sin(v + 2pi / 3)^2");
            sprintf(gSurfRange, "0 <= u <= 2 pi, 0 <= v <= 2 pi");
            // reverse u and v for better distribution or points
            temp = u;
            u = v + PI;
            v = temp + PI;  // convert to: 0 <= u <= 2 pi, 0 <= v <= 2 pi
            p.x = (2 + cos(u)) * pow(cos(v), 3) * sin(v);
            p.y = (2 + cos(u + TWOPI / 3)) * pow(cos(v + TWOPI / 3), 2) * pow(sin(v + TWOPI / 3), 2);
            p.z = -(2 + cos(u - TWOPI / 3)) * pow(cos(v + TWOPI / 3), 2) * pow(sin(v + TWOPI / 3), 2);
            break;
        case 3:
            sprintf(gSurfName, "Slippers Surface");
            sprintf(gSurfCredit, "by Roger Bagula");
            sprintf(gSurfX, "x =  (2 + cos(u)) cos(v)^3 sin(v)");
            sprintf(gSurfY, "y =  (2 + cos(u + 2pi / 3)) cos(2pi / 3 + v)^2 sin(2pi / 3 + v)^2");
            sprintf(gSurfZ, "z = -(2 + cos(u - 2pi / 3)) cos(2pi / 3 - v)^2 sin(2pi / 3 - v)^3");
            sprintf(gSurfRange, "0 <= u <= 2 pi, 0 <= v <= 2 pi");
            temp = u;
            u = v + PI * 2;
            v = temp + PI;  // convert to: 0 <= u <= 4 pi, 0 <= v <= 2 pi
            p.x = (2 + cos(u)) * pow(cos(v), 3) * sin(v);
            p.y = (2 + cos(u + TWOPI / 3)) * pow(cos(TWOPI / 3 + v), 2) * pow(sin(TWOPI / 3 + v), 2);
            p.z = -(2 + cos(u - TWOPI / 3)) * pow(cos(TWOPI / 3 - v), 2) * pow(sin(TWOPI / 3 - v), 3);
            break;
        case 4:
            sprintf(gSurfName, "Maeder's Owl");
            sprintf(gSurfCredit, "by R. Maeder");
            sprintf(gSurfX, "x = v cos(u) - 0.5 v^2 cos(2 u)");
            sprintf(gSurfY, "y = - v sin(u) - 0.5 v^2 sin(2 u) ");
            sprintf(gSurfZ, "z = 4 v^1.5 cos(3 u / 2) / 3 ");
            sprintf(gSurfRange, "0 <= u <= 4 pi, 0 <= v <= 1");
            u = (u + PI) * 2;
            v = (v + PI) / TWOPI;  // convert to: 0 <= u <= 4 pi, 0 <= v <= 1
            p.x = v * cos(u) - 0.5 * v * v * cos(2 * u);
            p.y = -v * sin(u) - 0.5 * v * v * sin(2 * u);
            p.z = 4 * pow(v, 1.5) * cos(1.5 * u) / 3;
            break;


            p.x = v * cos(u) - 0.5 * v * v * cos(2 * u);
            p.y = -v * sin(u) - 0.5 * v * v * sin(2 * u);
            p.z = 4 * pow(v, 1.5) * cos(1.5 * u) / 3;
    }


    return (p);
}

void BuildGeometry(void) {
    if (gPolyList)
        glDeleteLists(gPolyList, 1);
    gPolyList = glGenLists(1);
    glNewList(gPolyList, GL_COMPILE);
    draw();
    glEndList();
}

void DrawT(GLint window_width, GLint window_height) {
    char outString[256] = "";
    GLint matrixMode;
    GLint vp[4];
    GLint lineSpacing = 13;
    GLint line = 0;
    GLint startOffest = 7;

    glGetIntegerv(GL_VIEWPORT, vp);
    glViewport(0, 0, window_width, window_height);

    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glScalef(2.0f / window_width, -2.0f / window_height, 1.0f);
    glTranslatef(-window_width / 2.0f, -window_height / 2.0f, 0.0f);

    // draw
    glColor3f(1.0, 1.0, 0.0);
    if (gShowInfo) {

        sprintf(outString, "Camera Position: (%0.1f, %0.1f, %0.1f)", gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z);
        DrawString(10, window_height - (lineSpacing * line++) - startOffest, outString);
        sprintf(outString, "Trackball Rotation: (%0.1f, %0.2f, %0.2f, %0.2f)", gTrackBallRotation[0], gTrackBallRotation[1], gTrackBallRotation[2], gTrackBallRotation[3]);
        DrawString(10, window_height - (lineSpacing * line++) - startOffest, outString);
        sprintf(outString, "World Rotation: (%0.1f, %0.2f, %0.2f, %0.2f)", gWorldRotation[0], gWorldRotation[1], gWorldRotation[2], gWorldRotation[3]);
        DrawString(10, window_height - (lineSpacing * line++) - startOffest, outString);
        sprintf(outString, "Aperture: %0.1f", gCamera.aperture);
        DrawString(10, window_height - (lineSpacing * line++) - startOffest, outString);
        if (gStereo)
            sprintf(outString, "Eye Separation: %0.2f", gCamera.eyeSep);
        else
            sprintf(outString, "Eye Separation: 0.0");
        DrawString(10, window_height - (lineSpacing * line++) - startOffest, outString);
        sprintf(outString, "Focus Distance: %0.1f", gCamera.focalLength);
        DrawString(10, window_height - (lineSpacing * line++) - startOffest, outString);
        sprintf(outString, "Vertices: %ld, Color Scheme: %ld", gSubDivisions * gIJRatio * gSubDivisions, gColorScheme);
        DrawString(10, window_height - (lineSpacing * line++) - startOffest, outString);
        {
            GLboolean lighting, twoSidedLighting, localViewer;
            lighting = glIsEnabled(GL_LIGHTING);
            glGetBooleanv(GL_LIGHT_MODEL_LOCAL_VIEWER, &localViewer);
            glGetBooleanv(GL_LIGHT_MODEL_TWO_SIDE, &twoSidedLighting);
            if (!gLighting) {
                sprintf(outString, "-- Lighting off");
            } else {
                if (!twoSidedLighting)
                    sprintf(outString, "-- Single Sided Lighting");
                else
                    sprintf(outString, "-- Two Sided Lighting");
                if (localViewer)
                    sprintf(outString, "%s: Local Viewer", outString);
            }
            DrawString(10, window_height - (lineSpacing * line++) - startOffest, outString);
        }
    }

    if (gShowHelp) {
        line = 1;
        sprintf(outString, "Controls:\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "left button drag: rotate camera\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "right (or crtl-left) button drag: dolly (zoom) camera\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "arrows: eye separation & focal length\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "-/+: aperture\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "L: toggle lighting\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "S: toggle stereo\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "H: toggle help\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "I: toggle info\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "C: toggle surface credits\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
        sprintf(outString, "[/]: decrease/increase visible area\n");
        DrawString(10, (lineSpacing * line++) + startOffest, outString);
    }

    if (gShowCredits) {
#define khStart 350
        line = 1;
        glColor3f(1.0, 1.0, 1.0);
        DrawString(window_width - khStart, (lineSpacing * line++) + startOffest, gSurfName);
        DrawString(window_width - khStart, (lineSpacing * line++) + startOffest, gSurfCredit);
        glColor3f(0.7, 0.7, 0.7);
        DrawString(window_width - khStart, (lineSpacing * line++) + startOffest, gSurfX);
        DrawString(window_width - khStart, (lineSpacing * line++) + startOffest, gSurfY);
        DrawString(window_width - khStart, (lineSpacing * line++) + startOffest, gSurfZ);
        DrawString(window_width - khStart, (lineSpacing * line++) + startOffest,
                gSurfRange);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(matrixMode);

    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

void DrawBlueLine(GLint window_width, GLint window_height) {
    GLint i;
    unsigned long buffer;

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    for (i = 0; i < 6; i++) glDisable(GL_CLIP_PLANE0 + i);
    glDisable(GL_COLOR_LOGIC_OP);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DITHER);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);
    glDisable(GL_SCISSOR_TEST);
#ifndef WIN32
    glDisable(GL_SHARED_TEXTURE_PALETTE_EXT);
#endif
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
#ifndef WIN32
    glDisable(GL_TEXTURE_3D);
    glDisable(GL_TEXTURE_CUBE_MAP);
    //glDisable(GL_TEXTURE_RECTANGLE_EXT);
    glDisable(GL_VERTEX_PROGRAM_ARB);
#endif

    for (buffer = GL_BACK_LEFT; buffer <= GL_BACK_RIGHT; buffer++) {
        GLint matrixMode;
        GLint vp[4];

        glDrawBuffer(buffer);

        glGetIntegerv(GL_VIEWPORT, vp);
        glViewport(0, 0, window_width, window_height);

        glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glScalef(2.0f / window_width, -2.0f / window_height, 1.0f);
        glTranslatef(-window_width / 2.0f, -window_height / 2.0f, 0.0f);

        // draw sync lines
        glColor3d(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);  // Draw a background line
        glVertex3f(0.0f, window_height - 0.5f, 0.0f);
        glVertex3f(window_width, window_height - 0.5f, 0.0f);
        glEnd();
        glColor3d(0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);  // Draw a line of the correct length (the cross over is about 40% across the screen from the left
        glVertex3f(0.0f, window_height - 0.5f, 0.0f);
        if (buffer == GL_BACK_LEFT)
            glVertex3f(window_width * 0.30f, window_height - 0.5f, 0.0f);
        else
            glVertex3f(window_width * 0.80f, window_height - 0.5f, 0.0f);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(matrixMode);

        glViewport(vp[0], vp[1], vp[2], vp[3]);
    }
    glPopAttrib();
}

void DrawBlueLine_Simple(GLint window_width, GLint window_height) {
    unsigned long buffer;

    for (buffer = GL_BACK_LEFT; buffer <= GL_BACK_RIGHT; buffer++) {
        GLint matrixMode;
        GLint vp[4];

        glDrawBuffer(buffer);

        glGetIntegerv(GL_VIEWPORT, vp);
        glViewport(0, 0, window_width, window_height);

        glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glScalef(2.0f / window_width, -2.0f / window_height, 1.0f);
        glTranslatef(-window_width / 2.0f, -window_height / 2.0f, 0.0f);

        // draw sync lines
        glColor3d(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);  // Draw a background line
        glVertex3f(0.0f, window_height - 0.5f, 0.0f);
        glVertex3f(window_width, window_height - 0.5f, 0.0f);
        glEnd();
        glColor3d(0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);  // Draw a line of the correct length (the cross over is about 40% across the screen from the left
        glVertex3f(0.0f, window_height - 0.5f, 0.0f);
        if (buffer == GL_BACK_LEFT)
            glVertex3f(window_width * 0.30f, window_height - 0.5f, 0.0f);
        else
            glVertex3f(window_width * 0.80f, window_height - 0.5f, 0.0f);
        glVertex3f(window_width, window_height - 0.5f, 0.0f);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(matrixMode);

        glViewport(vp[0], vp[1], vp[2], vp[3]);
    }
}

void init(void) {
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
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    gCamera.screenWidth = w;
    gCamera.screenHeight = h;
    glutPostRedisplay();
}


// #### ADDED FUNCTIONS FOR SHADING ####
void setupMatrices(float position_x, float position_y, float position_z, float lookAt_x, float lookAt_y, float lookAt_z) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, RENDER_WIDTH / RENDER_HEIGHT, 10, 40000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(position_x, position_y, position_z, lookAt_x, lookAt_y, lookAt_z, 0, 1, 0);
}


void generateShadowFBO() {
    int shadowMapWidth = RENDER_WIDTH * SHADOW_MAP_RATIO;
    int shadowMapHeight = RENDER_HEIGHT * SHADOW_MAP_RATIO;

    //GLfloat borderColor[4] = {0,0,0,0};

    GLenum FBOstatus;

    // Try to use a texture depth component
    glGenTextures(1, &depthTextureId);
    glBindTexture(GL_TEXTURE_2D, depthTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available
    //You can also try GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32 for the internal format.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // create a framebuffer object
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, fboId);

    // Instruct openGL that we won't bind a color texture with the currently binded FBO
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // attach the texture to FBO depth attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTextureId, 0);

    // check FBO status
    FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
    if (FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
        printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

    // switch back to window-system-provided framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}


void setTextureMatrix(void) {
    static double modelView[16];
    static double projection[16];

    // This is matrix transform every coordinate x,y,z
    // x = x* 0.5 + 0.5
    // y = y* 0.5 + 0.5
    // z = z* 0.5 + 0.5
    // Moving from unit cube [-1,1] to [0,1]
    const GLdouble bias[16] = {
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0};

    // Grab modelview and transformation matrices
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);


    glMatrixMode(GL_TEXTURE);
    glActiveTexture(GL_TEXTURE7);

    glLoadIdentity();
    glLoadMatrixd(bias);

    // concatating all matrice into one.
    glMultMatrixd(projection);
    glMultMatrixd(modelView);

    // Go back to normal matrix mode
    glMatrixMode(GL_MODELVIEW);
}


GLhandleARB loadShader(char *filename, unsigned int type) {
    FILE *pfile;
    GLhandleARB handle;
    const GLcharARB *files[1];

    // shader Compilation variable
    GLint result;  // Compilation code result
    GLint errorLoglength;
    char *errorLogText;
    GLsizei actualErrorLogLength;

    char buffer[400000];
    memset(buffer, 0, 400000);

    // This will raise a warning on MS compiler
    pfile = fopen(filename, "rb");
    if (!pfile) {
        printf("Sorry, can't open file: '%s'.\n", filename);
        exit(0);
    }

    fread(buffer, sizeof(char), 400000, pfile);
    //printf("%s\n",buffer);


    fclose(pfile);

    handle = glCreateShaderObjectARB(type);
    if (!handle) {
        //We have failed creating the vertex shader object.
        printf("Failed creating vertex shader object from file: %s.", filename);
        exit(0);
    }

    files[0] = (const GLcharARB *)buffer;
    glShaderSourceARB(
            handle,  //The handle to our shader
            1,       //The number of files.
            files,   //An array of const char * data, which represents the source code of theshaders
            NULL);

    glCompileShaderARB(handle);

    //Compilation checking.
    glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);

    // If an error was detected.
    if (!result) {
        //We failed to compile.
        printf("Shader '%s' failed compilation.\n", filename);

        //Attempt to get the length of our error log.
        glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);

        //Create a buffer to read compilation error message
        errorLogText = (char *)malloc(sizeof(char) * errorLoglength);

        //Used to get the final length of the log.
        glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);

        // Display errors.
        printf("%s\n", errorLogText);

        // Free the buffer malloced earlier
        free(errorLogText);
    }

    return handle;
}


void loadShadowShader() {
    GLhandleARB vertexShaderHandle;
    GLhandleARB fragmentShaderHandle;

    vertexShaderHandle = loadShader("VertexShader.vert", GL_VERTEX_SHADER);
    fragmentShaderHandle = loadShader("FragmentShader.frag", GL_FRAGMENT_SHADER);

    shadowShaderId = glCreateProgramObjectARB();

    glAttachObjectARB(shadowShaderId, vertexShaderHandle);
    glAttachObjectARB(shadowShaderId, fragmentShaderHandle);
    glLinkProgramARB(shadowShaderId);

    shadowMapUniform = glGetUniformLocationARB(shadowShaderId, "ShadowMap");
}


void render_with_lighting() {
    //First step: Render from the light POV to a FBO, story depth values only
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, fboId);  //Rendering offscreen

    //Using the fixed pipeline to render to the depthbuffer
    glUseProgramObjectARB(0);

    // In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
    glViewport(0, 0, RENDER_WIDTH * SHADOW_MAP_RATIO, RENDER_HEIGHT * SHADOW_MAP_RATIO);

    // Clear previous frame values
    glClear(GL_DEPTH_BUFFER_BIT);

    //Disable color rendering, we only want to write to the Z-Buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // setupMatrices(p_light[0], p_light[1], p_light[2], l_light[0], l_light[1], l_light[2]);
    gluLookAt(p_light[0], p_light[1], p_light[2],
            p_light[0] + gCamera.viewDir.x,
            p_light[1] + gCamera.viewDir.y,
            p_light[2] + gCamera.viewDir.z,
            gCamera.viewUp.x, gCamera.viewUp.y, gCamera.viewUp.z);

    // Culling switching, rendering only backface, this is done to avoid self-shadowing
    glCullFace(GL_FRONT);
    glCallList(gPolyList);

    // Save modelview/projection matrice into texture7, also add a biais
    setTextureMatrix();

    // Now rendering from the camera POV, using the FBO to generate shadows
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

    glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);

    // Enabling color write (previously disabled for light POV z-buffer rendering)
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Clear previous frame values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Using the shadow shader
    glUseProgramObjectARB(shadowShaderId);
    glUniform1i(shadowMapUniform, 7);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, depthTextureId);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z,
            gCamera.viewPos.x + gCamera.viewDir.x,
            gCamera.viewPos.y + gCamera.viewDir.y,
            gCamera.viewPos.z + gCamera.viewDir.z,
            gCamera.viewUp.x, gCamera.viewUp.y, gCamera.viewUp.z);

    //glTranslatef(gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z);
    glRotatef(gTrackBallRotation[0], gTrackBallRotation[1], gTrackBallRotation[2], gTrackBallRotation[3]);
    //glTranslatef(-gCamera.viewPos.x, -gCamera.viewPos.y, -gCamera.viewPos.z);
    glRotatef(gWorldRotation[0], gWorldRotation[1], gWorldRotation[2], gWorldRotation[3]);

    glCullFace(GL_BACK);
    glCallList(gPolyList);
}

// ############################


void display(void) {
    recVec r;
    GLdouble ratio, radians, wd2, ndfl;
    GLdouble left, right, top, bottom, nearc, farc;

    nearc = -gCamera.viewPos.z - gShapeSize * 0.5;
    if (nearc < 0.1)
        nearc = 0.1;
    farc = -gCamera.viewPos.z + gShapeSize * 0.5;

    // Misc stuff
    ratio = gCamera.screenWidth / (double)gCamera.screenHeight;
    radians = DTOR * gCamera.aperture / 2;
    wd2 = nearc * tan(radians);
    ndfl = nearc / gCamera.focalLength;

    // Derive the two eye positions
    CROSSPROD(gCamera.viewDir, gCamera.viewUp, r);
    normalise(&r);

    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (gStereo) {
        left = -ratio * wd2 + 0.5 * gCamera.eyeSep * ndfl;
        right = ratio * wd2 + 0.5 * gCamera.eyeSep * ndfl;
    } else {
        left = -ratio * wd2;
        right = ratio * wd2;
    }
    top = wd2;
    bottom = -wd2;
    glFrustum(left, right, bottom, top, nearc, farc);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z,
            gCamera.viewPos.x + gCamera.viewDir.x,
            gCamera.viewPos.y + gCamera.viewDir.y,
            gCamera.viewPos.z + gCamera.viewDir.z,
            gCamera.viewUp.x, gCamera.viewUp.y, gCamera.viewUp.z);

    // track ball rotation
    // translate object

    //glTranslatef(gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z);
    glRotatef(gTrackBallRotation[0], gTrackBallRotation[1], gTrackBallRotation[2], gTrackBallRotation[3]);
    //glTranslatef(-gCamera.viewPos.x, -gCamera.viewPos.y, -gCamera.viewPos.z);
    glRotatef(gWorldRotation[0], gWorldRotation[1], gWorldRotation[2], gWorldRotation[3]);

    // render_with_lighting();
    if (gPolygons) {
        if (gLighting)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
        glDisable(GL_LIGHTING);
        render_with_lighting();
        // glCallList(gPolyList);
        glDisable(GL_LIGHTING);
    }
    DrawT(gCamera.screenWidth, gCamera.screenHeight);

    // #### MIRROR STUFF ####
    // Reference point on mirror
    static GLfloat reference_point[] = {252.589f, 105.177f, 172.436f};
    static GLfloat normal_vector[] = {0.00000f, 0.000000f, 1.000000f};

    glUseProgramObjectARB(0);

    // Set up stencil
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glClearStencil(0x00);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // Select mirror area for stencil
    mesh242();
    mesh243();

    glStencilFunc(GL_EQUAL, 0x1, 0x1);

    // Reflect the room
    glPushMatrix();
    glTranslatef(0, 0, reference_point[2] * 2);
    glScalef(1, 1, -1);

    // Render inside stencil
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CLIP_PLANE0);
    double plane[] = {0, 0, 1, -172.436};
    glClipPlane(GL_CLIP_PLANE0, plane);
    // render_with_lighting();
    if (gPolygons) {
        if (gLighting)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
        glCallList(gPolyList);
        glDisable(GL_LIGHTING);
    }
    glDisable(GL_CLIP_PLANE0);

    // Flip the room back
    glPopMatrix();

    // Disable stencil
    glDisable(GL_STENCIL_TEST);

    // ####################

    glutSwapBuffers();
}

inline float toFloat(int x) {
    return 0.02 * (x / 32768.0);
}

void animate(void) {
    //static int x;
    //static int y;

    Point3D move;
    Point3D cDir = gCamera.viewDir;
    double turn = 0.0;
    static const double step = 0.1;
#ifdef USE_JOYSTICK
    //Joystick
    js.Input();
    BasicJoystick::Point p = js.GetAxis(0);
    move.x -= toFloat(p.x);
    move.y -= toFloat(p.y);
    p = js.GetAxis(1);
    turn -= toFloat(p.x);
    move.z -= toFloat(p.y);
#endif
#ifdef USE_WII
    //Wii
    wii.Poll();
    if (wii.IsPressed(Wii::ButtonLeft)) turn += step;
    if (wii.IsPressed(Wii::ButtonRight)) turn -= step;
    if (wii.IsPressed(Wii::ButtonUp)) move.z += step;
    if (wii.IsPressed(Wii::ButtonDown)) move.z -= step;
#endif
    //Adjust Camera
    gCamera.viewDir.x += turn * jsign * 0.25;
    if (gCamera.viewDir.x > 1.0) {
        gCamera.viewDir.x = 1.0;
        jsign *= -1;
    }
    if (gCamera.viewDir.x < -1.0) {
        gCamera.viewDir.x = -1.0;
        jsign *= -1;
    }
    gCamera.viewDir.z = jsign * sqrt(1 - (gCamera.viewDir.x * gCamera.viewDir.x));
    normalise(&gCamera.viewDir);
    Point3D up = Point3D(gCamera.viewUp).Normalize();
    Point3D forward = Point3D(gCamera.viewDir).Normalize();
    Point3D side = Point3D(up * forward).Normalize();
    Point3D camera = gCamera.viewPos;
    camera += up.Scale(move.y);
    camera += forward.Scale(move.z);
    camera += side.Scale(move.x);
    camera.CopyTo(gCamera.viewPos);

#ifdef WIN32
    Sleep(1);
#else
    usleep(10);
#endif

    glutPostRedisplay();
}

void special(int key, int px, int py) {
    gLastKey = key;
    switch (key) {
        case GLUT_KEY_UP:  // arrow forward, close in on world
            gCamera.focalLength -= 0.5f;
            if (gCamera.focalLength < 0.0f)
                gCamera.focalLength = 0.0f;
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:  // arrow back, back away from world
            gCamera.focalLength += 0.5f;
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT:  // arrow left, close eyes together
            gCamera.eyeSep -= 0.005f;
            if (gCamera.eyeSep < 0.0)
                gCamera.eyeSep = 0.0;
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT:  // arrow right, spread eyes apart
            gCamera.eyeSep += 0.005f;
            glutPostRedisplay();
            break;
    }
}

void mouseDolly(int x, int y) {
    if (gDolly) {
        GLfloat dolly = (gDollyPanStartPoint[1] - y) * -gCamera.viewPos.z / 200.0f;
        GLfloat eyeRelative = gCamera.eyeSep / gCamera.focalLength;
        gCamera.focalLength += gCamera.focalLength / gCamera.viewPos.z * dolly;
        if (gCamera.focalLength < 1.0)
            gCamera.focalLength = 1.0;
        gCamera.eyeSep = gCamera.focalLength * eyeRelative;
        gCamera.viewPos.z += dolly;
        if (gCamera.viewPos.z == 0.0)  // do not let z = 0.0
            gCamera.viewPos.z = 0.0001;
        gDollyPanStartPoint[0] = x;
        gDollyPanStartPoint[1] = y;
        glutPostRedisplay();
    }
}

void mousePan(int x, int y) {
    if (gPan) {
        GLfloat panX = (gDollyPanStartPoint[0] - x) / (900.0f / -gCamera.viewPos.z);
        GLfloat panY = (gDollyPanStartPoint[1] - y) / (900.0f / -gCamera.viewPos.z);
        gCamera.viewPos.x -= panX;
        gCamera.viewPos.y -= panY;
        gDollyPanStartPoint[0] = x;
        gDollyPanStartPoint[1] = y;
        glutPostRedisplay();
    }
}

void mouseTrackball(int x, int y) {
    if (gTrackBall) {
        rollToTrackball(x, y, gTrackBallRotation);
        glutPostRedisplay();
    }
}

void mouse(int gbutton, int gstate, int x, int y) {
    button = gbutton;
    state = gstate;

    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
        if (gDolly) {  // if we are currently dollying, end dolly
            mouseDolly(x, y);
            gDolly = GL_FALSE;
            glutMotionFunc(NULL);
            gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
            glutMotionFunc(NULL);
        } else if (gPan) {
            mousePan(x, y);
            gPan = GL_FALSE;
            glutMotionFunc(NULL);
            gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
            glutMotionFunc(NULL);
        }
        startTrackball(x, y, 0, 0, gCamera.screenWidth, gCamera.screenHeight);
        glutMotionFunc(mouseTrackball);
        gTrackBall = GL_TRUE;
    } else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {
        gTrackBall = GL_FALSE;
        glutMotionFunc(NULL);
        rollToTrackball(x, y, gTrackBallRotation);
        if (gTrackBallRotation[0] != 0.0)
            addToRotationTrackball(gTrackBallRotation, gWorldRotation);
        gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
        glutPostRedisplay();
    } else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN)) {
        if (gTrackBall) {  // if we are currently trackballing, end trackball
            gTrackBall = GL_FALSE;
            glutMotionFunc(NULL);
            rollToTrackball(x, y, gTrackBallRotation);

            if (gTrackBallRotation[0] != 0.0)
                addToRotationTrackball(gTrackBallRotation, gWorldRotation);
            gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
        } else if (gPan) {
            mousePan(x, y);
            gPan = GL_FALSE;
            glutMotionFunc(NULL);
            gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
            glutMotionFunc(NULL);
        }
        gDollyPanStartPoint[0] = x;
        gDollyPanStartPoint[1] = y;
        glutMotionFunc(mouseDolly);
        gDolly = GL_TRUE;
    } else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP)) {
        mouseDolly(x, y);
        gDolly = GL_FALSE;
        glutMotionFunc(NULL);
        gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
        glutMotionFunc(NULL);
    } else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
        if (gTrackBall) {  // if we are currently trackballing, end trackball
            gTrackBall = GL_FALSE;
            glutMotionFunc(NULL);
            rollToTrackball(x, y, gTrackBallRotation);
            if (gTrackBallRotation[0] != 0.0)
                addToRotationTrackball(gTrackBallRotation, gWorldRotation);
            gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
        } else if (gDolly) {
            mouseDolly(x, y);
            gDolly = GL_FALSE;
            glutMotionFunc(NULL);
            gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
            glutMotionFunc(NULL);
        }
        gDollyPanStartPoint[0] = x;
        gDollyPanStartPoint[1] = y;
        glutMotionFunc(mousePan);
        gPan = GL_TRUE;
    } else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)) {
        mousePan(x, y);
        gPan = GL_FALSE;
        glutMotionFunc(NULL);
        gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
        glutMotionFunc(NULL);
    }
}

void motion(int x, int y) {
    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
        gTrackBall = GL_FALSE;
        glutMotionFunc(NULL);
        rollToTrackball(x, y, gTrackBallRotation);
        if (gTrackBallRotation[0] != 0.0)
            addToRotationTrackball(gTrackBallRotation, gWorldRotation);
        gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
        glutPostRedisplay();
    } else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN)) {
        mouseDolly(x, y);
        gDolly = GL_FALSE;
        glutMotionFunc(NULL);
        gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
        glutMotionFunc(NULL);
    } else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
        mousePan(x, y);
        gPan = GL_FALSE;
        glutMotionFunc(NULL);
        gTrackBallRotation[0] = gTrackBallRotation[1] = gTrackBallRotation[2] = gTrackBallRotation[3] = 0.0f;
        glutMotionFunc(NULL);
    }
}

void key(unsigned char inkey, int px, int py) {
    gLastKey = inkey;
    switch (inkey) {
        case 27:
            exit(0);
            break;
        case 'h':  // point
        case 'H':
            gShowHelp = 1 - gShowHelp;
            glutPostRedisplay();
            break;
        case 'i':  // point
        case 'I':
            gShowInfo = 1 - gShowInfo;
            glutPostRedisplay();
            break;
        case 'c':  // point
        case 'C':
            gShowCredits = 1 - gShowCredits;
            glutPostRedisplay();
            break;
        case 's':  // stereo
        case 'S':
            gStereo = 1 - gStereo;
            if (gStereo) {  // switch to full screen
                glutSetWindow(gMainWindow);
                glutHideWindow();
                glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);  // stereo display mode for glut
                glutGameModeString("1360x768:24@60");                                    // must now use full screen game mode
                glutEnterGameMode();                                                     // enter gamemode to get stereo context (may get invalid drawable warnings in console, this is normal and will be fixed in
                init();                                                                  // ensure gl is setup since this is a new window
                glutReshapeFunc(reshape);
                glutDisplayFunc(display);
                glutKeyboardFunc(key);
                glutSpecialFunc(special);
                glutMouseFunc(mouse);
                glutPassiveMotionFunc(motion);
            } else {
                glutLeaveGameMode();
                glutSetWindow(gMainWindow);
                glutShowWindow();
                gCamera.screenWidth = glutGet(GLUT_WINDOW_WIDTH);
                gCamera.screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
            }
            break;
        case 'l':    // stereo
        case 'L': {  // loop though off, one sided, one sided local, two sided, two sided local
            gLighting++;
            if (gLighting > 4)
                gLighting = 0;
            SetLighting();
            glutPostRedisplay();
        } break;
        case '=':  // increase camera aperture
        case '+':
            gCamera.aperture += 0.5f;
            glutPostRedisplay();
            break;
        case '-':  // decrease camera aperture
        case '_':
            gCamera.aperture -= 0.5f;
            if (gCamera.aperture < 0.0f)
                gCamera.aperture = 0.0f;
            glutPostRedisplay();
            break;
        case '[':
        case '{':
            gShapeSize *= 0.9;
            glutPostRedisplay();
            break;
        case ']':
        case '}':
            gShapeSize *= 1.1;
            glutPostRedisplay();
            break;
        case '~':
            break;
    }
}

void entry(int state) {
    if (state != GLUT_ENTERED) {
        button = state = -1;
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, (char **)argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);  // non-stereo for main window
    glutInitWindowSize(RENDER_WIDTH, RENDER_HEIGHT);
    gMainWindow = glutCreateWindow("[floating] CEG4500 Project 5");

    // #### ADDED FOR SHADING ####
    glewInit();
    generateShadowFBO();
    loadShadowShader();

    // This is important, if not here, FBO's depthbuffer won't be populated.
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1.0f);

    glEnable(GL_CULL_FACE);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    // ############################


    init();
    jsign = 1.0;

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(animate);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    startTrackball(0, 0, 0, 0, gCamera.screenWidth, gCamera.screenHeight);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(motion);

    glutMainLoop();
    return 0;
}
