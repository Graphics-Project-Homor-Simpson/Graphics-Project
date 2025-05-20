#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glut.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#define NUM_TEXTURES 1
#endif

GLuint textures[NUM_TEXTURES]; // 텍스처 전역 변수

// 카메라 제어
float cameraRadius = 3.0f;
float cameraTheta = 1.0f;
float cameraPhi = 1.5f;
int mouseX, mouseY;
bool isDragging = false;

void updateCameraSpace() {
    float eyeX = cameraRadius * sinf(cameraPhi) * cosf(cameraTheta);
    float eyeY = cameraRadius * cosf(cameraPhi);
    float eyeZ = cameraRadius * sinf(cameraPhi) * sinf(cameraTheta);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


// 몸통
void drawHomerTorso() {
    const int stacks = 100;
    const int slices = 100;
    float height = 1.2f;

    float radius_profile[stacks + 1];
    for (int i = 0; i <= stacks; ++i) {
        float t = (float)i / stacks;
        radius_profile[i] = 0.26f
            + 0.20f * pow(sin((1 - t) * 0.85f * M_PI), 2.3f)
            + 0.24f * pow(1 - t, 1.88f)
            + 0.013f * exp(-pow((t - 0.09f) * 11.0f, 2.9f))
            - 0.06f * pow(t, 3.0f);
    }

    glPushMatrix();
    glTranslatef(0.0f, -0.6f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    for (int i = 0; i < stacks; ++i) {
        float z0 = height * i / stacks;
        float z1 = height * (i + 1) / stacks;
        float r0 = radius_profile[i];
        float r1 = radius_profile[i + 1];

        float v0 = (float)i / stacks;
        float v1 = (float)(i + 1) / stacks;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * M_PI * j / slices;
            float x = cos(theta);
            float y = sin(theta);
            float u = (float)j / slices;

            glNormal3f(x, y, 0);
            glTexCoord2f(u, v0); glVertex3f(r0 * x, r0 * y, z0);
            glTexCoord2f(u, v1); glVertex3f(r1 * x, r1 * y, z1);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D); // 곡면 텍스처 끝

    // 밑판 (흰색 원)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.115f);
    glRotatef(90, 0, 0, 1);
    glColor3f(1.0f, 1.0f, 1.0f);
    float r = radius_profile[stacks] * 2.75;
    GLUquadric* disk = gluNewQuadric();
    gluDisk(disk, 0.0, r, 64, 1);
    gluDeleteQuadric(disk);
    glPopMatrix();

    
    // [2] 목을 원판으로 막기
    float neckRadius = radius_profile[stacks];
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);  // 몸통 맨 위 위치
    glColor3f(1.0f, 0.85f, 0.2f);     // 노란색 (피부색)
    GLUquadric* neckDisk = gluNewQuadric();
    gluDisk(neckDisk, 0.0, neckRadius, 32, 1);
    gluDeleteQuadric(neckDisk);
    glPopMatrix();

    glPopMatrix();
}

// 조명
void initLighting() {
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat diffuse[] = { 0.8, 0.8, 0.8, 1.0 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

// 디스플레이
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    updateCameraSpace();
    glEnable(GL_NORMALIZE);
    glColor3f(1.0f, 1.0f, 1.0f);  // 기본색

    glPushMatrix();
    glRotatef(120, 0, 1, 0);  // 정면 방향 돌리기
    drawHomerTorso();
    glPopMatrix();

    glutSwapBuffers();
}

// 리쉐이프
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
}

// 마우스 입력
void inputMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseX = x;
        mouseY = y;
        isDragging = true;
    }
    else {
        isDragging = false;
    }
}

// 드래그 회전
void dragMouse(int x, int y) {
    if (isDragging) {
        float dx = (x - mouseX) * 0.005f;
        float dy = (y - mouseY) * 0.005f;
        cameraTheta += dx;
        cameraPhi -= dy;
        if (cameraPhi < 0.1f) cameraPhi = 0.1f;
        if (cameraPhi > 3.04f) cameraPhi = 3.04f;
        mouseX = x;
        mouseY = y;
        glutPostRedisplay();
    }
}

// 줌
void inputKeyboard(unsigned char key, int x, int y) {
    if (key == 'a') {
        cameraRadius -= 0.1f;
        if (cameraRadius < 1.0f) cameraRadius = 1.0f;
    }
    else if (key == 'z') {
        cameraRadius += 0.1f;
    }
    glutPostRedisplay();
}

void loadTexture(const char* filepath, GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (data) {

        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3) // R, G, B (jpg)
            format = GL_RGB;
        else if (nrChannels == 4) // PNG
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //텍스쳐로드 성공시.
        std::cout << "텍스쳐 로드 성공" << std::endl;
        stbi_image_free(data);
    }
}

void init() {
    //컬링 및 뎁스, 페이스 방향 설정
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //텍스쳐 설정
    glEnable(GL_TEXTURE_2D);

    //텍스쳐 버퍼 생성
    glGenTextures(1, textures);

    //텍스쳐 로드 함수로 텍스쳐 로드
    loadTexture("shirts.png", textures[0]);

    // 텍스처가 컬러에 섞이지 않고 순수하게 보여지도록 한다.
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}


// 메인 함수
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Homer Torso – Final");

    initLighting();
    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(inputMouse);
    glutMotionFunc(dragMouse);
    glutKeyboardFunc(inputKeyboard);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glutMainLoop();
    return 0;
}
