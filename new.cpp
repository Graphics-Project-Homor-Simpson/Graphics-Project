#include <glut.h>    // 또는 macOS에서는 <GLUT/glut.h>
#include <freeglut.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 마우스 변수
int mouseX, mouseY;

//구면 좌표계 활용
float cameraRadius = 5.0; // 원점 중심으로 하는 Sphere 반지름 설정
float cameraTheta = 1.0;
float cameraPhi = 1.57;

//드래깅 여부
bool isDragging;

GLUquadric* quad = gluNewQuadric();
float head_radius = 0.45f;
float head_height = 0.65f;
    


void updateCameraSpace() {
    float cameraEyeX = cameraRadius * sinf(cameraPhi) * cosf(cameraTheta);
    float cameraEyeY = cameraRadius * cosf(cameraPhi);
    float cameraEyeZ = cameraRadius * sinf(cameraPhi) * sinf(cameraTheta);

    glLoadIdentity();
    gluLookAt(cameraEyeX, cameraEyeY, cameraEyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// ------------------- Reshape -------------------
void reshape(int w, int h) {
    // 뷰포트 설정 
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Perspective -> 원근 투영
    gluPerspective(60.0, (float)w / h, 0.01, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat lightPos[] = { 1.0f, 1.0f, 2.0f, 1.0f };
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);

    quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
}

// ------------------- Head -------------------
void drawHead() {
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.85f, 0.2f);
    gluCylinder(quad, head_radius, head_radius, head_height, 40, 40);
    gluDisk(quad, 0.0f, head_radius, 40, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, head_height, 0.0f);
    glColor3f(1.0f, 0.85f, 0.2f);
    glutSolidSphere(head_radius, 40, 40);
    glPopMatrix();
}

// ------------------- Eyes -------------------
void drawEyes() {
    float eyey = 0.52f;
    float eyez = 0.38f;
    float eye_offset_x = 0.125f;

    glPushMatrix();
    glTranslatef(-eye_offset_x, eyey, eyez);
    glScalef(1.0f, 1.0f, 0.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidSphere(0.18, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(eye_offset_x, eyey, eyez);
    glScalef(1.0f, 1.0f, 0.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidSphere(0.18, 30, 30);
    glPopMatrix();
}

// ------------------- Nose -------------------
void drawNose() {
    float nose_radius = 0.05f;
    float nose_length = 0.12f;
    float base_y = 0.42f;

    glPushMatrix();
    glTranslatef(0.0f, base_y, 0.45f);
    glColor3f(1.0f, 0.85f, 0.2f);
    gluCylinder(quad, nose_radius, nose_radius, nose_length, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, base_y, 0.45f + nose_length);
    glColor3f(1.0f, 0.85f, 0.2f);
    glutSolidSphere(nose_radius, 20, 20);
    glPopMatrix();
}

// ------------------- Mouth -------------------
void drawMouthArea() {
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.25f);
    glScalef(2.2f, 1.5f, 1.5f);
    glColor3f(0.7f, 0.6f, 0.5f);
    glutSolidSphere(0.2, 30, 30);
    glPopMatrix();
}

// ------------------- Hair -------------------
void drawHairLines() {
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(7.0f);

    float baseY = head_height + head_radius - 0.12f;

    glBegin(GL_LINE_STRIP);
    for (float t = 0; t <= M_PI; t += 0.05f) {
        float x = -0.05f + 0.3f * cos(t);
        float y = baseY + 0.18f * sin(t);
        float z = -0.005f;
        glVertex3f(x, y, z);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (float t = 0; t <= M_PI; t += 0.05f) {
        float x = 0.06f + 0.25f * cos(t);
        float y = baseY + 0.14f * sin(t);
        float z = -0.005f;
        glVertex3f(x, y, z);
    }
    glEnd();

    glPopMatrix();
}

// ------------------- Half Torus -------------------
void drawHalfTorus(float outerRadius, float innerRadius, int sides, int rings) {
    float twoPi = 2.0f * M_PI;
    for (int i = 0; i <= rings / 2; ++i) {
        float theta = i / (float)rings * twoPi;
        float nextTheta = (i + 1) / (float)rings * twoPi;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= sides; ++j) {
            float phi = j / (float)sides * twoPi;
            float cosPhi = cos(phi);
            float sinPhi = sin(phi);

            float x0 = (outerRadius + innerRadius * cosPhi) * cos(theta);
            float y0 = (outerRadius + innerRadius * cosPhi) * sin(theta);
            float z0 = innerRadius * sinPhi;

            float x1 = (outerRadius + innerRadius * cosPhi) * cos(nextTheta);
            float y1 = (outerRadius + innerRadius * cosPhi) * sin(nextTheta);
            float z1 = z0;

            glVertex3f(x0, y0, z0);
            glVertex3f(x1, y1, z1);
        }
        glEnd();
    }
}

// ------------------- Ears -------------------
void drawEars() {
    float ear_y = 0.34f;
    float ear_z = 0.05f;
    float ear_offset_x = 0.4f;

    glPushMatrix();
    glTranslatef(-ear_offset_x, ear_y, ear_z);
    glRotatef(180, -1, 1, 0);
    glColor3f(1.0f, 0.85f, 0.2f);
    drawHalfTorus(0.1f, 0.02f, 20, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(ear_offset_x, ear_y, ear_z);
    glRotatef(180, 1, 1, 0);
    glColor3f(1.0f, 0.85f, 0.2f);
    drawHalfTorus(0.1f, 0.02f, 20, 30);
    glPopMatrix();
}

// ------------------- Display -------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /*glLoadIdentity();*/

    gluLookAt(0.0, 0.0, -1.0,   // eye
        0.0, 0.0, 0.0,   // center
        0.0, 1.0, 0.0);  // up

    drawHead();
    drawEyes();
    drawNose();
    drawMouthArea();
    drawHairLines();
    drawEars();

    glutSwapBuffers();
}

// ------------------- Main -------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Homer Simpson");

    init();

    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    //마우스 입력 && 드래깅
    glutMouseFunc(inputMouse);
    glutMotionFunc(dragMouse);

    // 마우스 줌인 줌아웃
    glutMouseWheelFunc(mouseWheel);

    //키보드 입력 ( 줌 인 / 줌 아웃)'
    glutKeyboardFunc(inputKeyboard);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutMainLoop();
    return 0;
}