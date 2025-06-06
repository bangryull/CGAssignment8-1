#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <cstdio>
#include <vector>

struct Vector3 {
    float x, y, z;
};

struct Triangle {
    unsigned int indices[3];
};

extern std::vector<Vector3> gPositions;
extern std::vector<Vector3> gNormals;
extern std::vector<Triangle> gTriangles;

float gTotalTimeElapsed = 0;
int gTotalFrames = 0;
GLuint gTimer;

void init_timer() {
    glGenQueries(1, &gTimer);
}

void start_timing() {
    glBeginQuery(GL_TIME_ELAPSED, gTimer);
}

float stop_timing() {
    glEndQuery(GL_TIME_ELAPSED);
    GLint available = GL_FALSE;
    while (available == GL_FALSE)
        glGetQueryObjectiv(gTimer, GL_QUERY_RESULT_AVAILABLE, &available);

    GLint result;
    glGetQueryObjectiv(gTimer, GL_QUERY_RESULT, &result);
    return result / (1000.0f * 1000.0f * 1000.0f); // nano to seconds
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 0,   // eye
        0, 0, -1,  // center
        0, 1, 0);  // up

    GLfloat globalAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    GLfloat light_pos[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // ✅ Material properties
    GLfloat ka[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat kd[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat ks[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    glPushMatrix();
    glTranslatef(0.1f, -1.0f, -1.5f);
    glScalef(10.0f, 10.0f, 10.0f);

    start_timing();

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < gTriangles.size(); ++i) {
        unsigned int k0 = gTriangles[i].indices[0];
        unsigned int k1 = gTriangles[i].indices[1];
        unsigned int k2 = gTriangles[i].indices[2];

        Vector3 n0 = gNormals[k0];
        Vector3 n1 = gNormals[k1];
        Vector3 n2 = gNormals[k2];

        // 수동 정규화 (필요하면)
        auto normalize = [](Vector3 v) {
            float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
            if (len == 0.0f) return Vector3{ 0, 0, 0 };
            return Vector3{ v.x / len, v.y / len, v.z / len };
            };

        n0 = normalize(n0);
        n1 = normalize(n1);
        n2 = normalize(n2);

        glNormal3f(n0.x, n0.y, n0.z);
        glVertex3f(gPositions[k0].x, gPositions[k0].y, gPositions[k0].z);

        glNormal3f(n1.x, n1.y, n1.z);
        glVertex3f(gPositions[k1].x, gPositions[k1].y, gPositions[k1].z);

        glNormal3f(n2.x, n2.y, n2.z);
        glVertex3f(gPositions[k2].x, gPositions[k2].y, gPositions[k2].z);
    }
    glEnd();

    float timeElapsed = stop_timing();
    gTotalFrames++;
    gTotalTimeElapsed += timeElapsed;
    float fps = gTotalFrames / gTotalTimeElapsed;

    char title[128];
    sprintf(title, "OpenGL Bunny: %.2f FPS", fps);
    glutSetWindowTitle(title);

    glPopMatrix();
    glutSwapBuffers();
    glutPostRedisplay();
}

