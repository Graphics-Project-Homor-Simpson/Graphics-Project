#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glut.h>
#include <freeglut.h>
#include <cmath>
#include <iostream>

//사운드 라이브러리
#include <Windows.h>
#include <conio.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")


#define PI 3.14159265358979323846
#define NUM_TEXTURES 3

//마우스 변수
int mouseX, mouseY;

//구면 좌표계 활용
float cameraRadius = 5.0; // 원점 중심으로 하는 Sphere 반지름 설정
float cameraTheta = 1.0;
float cameraPhi = 1.57;

// 텍스처 전역 변수
GLuint textures[NUM_TEXTURES];

//드래깅 여부
bool isDragging;

// Function 
void legPart();
void drawHalfSphere(float radius, int slices, int stacks);
void drawCylinder(float radius, float height, int segments);
void drawHalfCapsule(float radius, float height, int slices, int stacks);
void drawHalfCircle(float radius, float y, int slices);
void drawingCube();
void drawSphere(float radius, int slices, int stacks);

GLUquadric* quad = gluNewQuadric();
float head_radius = 0.45f;
float head_height = 0.65f;


// Camera ---------------------------------------------------------------------------
void updateCameraSpace() {
    float cameraEyeX = cameraRadius * sinf(cameraPhi) * cosf(cameraTheta);
    float cameraEyeY = cameraRadius * cosf(cameraPhi);
    float cameraEyeZ = cameraRadius * sinf(cameraPhi) * sinf(cameraTheta);

    glLoadIdentity();
    gluLookAt(cameraEyeX, cameraEyeY, cameraEyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
// Reshape --------------------------------------------------------------------------
void reshape(int w, int h) {
    // 뷰포트 설정 
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Perspective -> 원근 투영
    gluPerspective(60.0, (float)w / h, 0.01, 100.0);

    glMatrixMode(GL_MODELVIEW);
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

// 몸통
void drawHomerTorso() {
    const int stacks = 100;
    const int slices = 100;
    float height = 1.2f;

    float radius_profile[stacks + 1];
    for (int i = 0; i <= stacks; ++i) {
        float t = (float)i / stacks;
        radius_profile[i] = 0.26f
            + 0.20f * pow(sin((1 - t) * 0.85f * PI), 2.3f)
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
            float theta = 2.0f * PI * j / slices;
            float x = cos(theta);
            float y = sin(theta);
            float u = (float)j / slices;

            glNormal3f(x, y, 0);

            glTexCoord2f(u, v1); glVertex3f(r1 * x, r1 * y, z1);
            glTexCoord2f(u, v0); glVertex3f(r0 * x, r0 * y, z0);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D); // 곡면 텍스처 끝

    // 밑판 (흰색 원) – 확실히 반시계 방향으로 직접 렌더링
    glPushMatrix();

    // [1] 위치 조정
    glTranslatef(0.0f, 0.0f, 0.115f);

    // [2] 시점 기준 회전 조정 (뒤집힌 방향으로)
    glRotatef(-90, 0, 0, 1);  // Z+ 위를 보도록 → 원판을 아래 향하게

    // [3] 컬링 꺼서 무조건 보이게
    glDisable(GL_CULL_FACE);

    // [4] 색상 및 원판 그리기
    glColor3f(1.0f, 1.0f, 1.0f);

    float r = radius_profile[stacks] * 2.75f;
    int numSegments = 64;

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f); // 중심
    for (int i = 0; i <= numSegments; ++i) {
        float theta = 2.0f * PI * (float)i / numSegments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex3f(x, y, 0.0f); // 반시계 방향
    }
    glEnd();

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


void drawHand(bool isLeft) {
    glPushMatrix();

    // 손바닥
    glColor3f(0.967f, 0.791f, 0.056f); // 노란색
    glutSolidSphere(0.16, 20, 20);     // 손바닥

    // 손가락 4개 (캡슐 형태)
    for (float i = -1.5f; i <= 1.5f; i += 1.0f) {
        glPushMatrix();

        // 손가락 위치 조정
        glTranslatef(i * 0.07f, 0.0f, 0.0f); // X축 위치 조정
        glRotatef(90, 0, 0, 1);              // Z축 기준 회전

        float radius = 0.03f;
        float length = 0.25f;

        // 색상 설정 (노란색)
        glColor3f(0.967f, 0.791f, 0.056f);

        // 아래쪽 반구
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        glutSolidSphere(radius, 16, 8);  // 아래 반구 (절반만 보여도 충분함)
        glPopMatrix();

        // 실린더 몸통
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, radius); // 아래 반구 위에 붙이기
        glutSolidCylinder(radius, length, 16, 8);
        glPopMatrix();

        // 위쪽 반구
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, radius + length); // 실린더 위 끝
        glutSolidSphere(radius, 16, 8);  // 위 반구
        glPopMatrix();

        glPopMatrix(); // 손가락 끝
    }

    glPopMatrix(); // 전체 손
}


//void drawHand(bool isLeft) {
//    glPushMatrix();
//
//    // 손바닥
//    glColor3f(0.967f, 0.791f, 0.056f); // 노란색
//    glutSolidSphere(0.16, 20, 20); // 손바닥
//
//    // 손가락 4개 (중앙 정렬, 균형 잡힌 간격)
//    for (float i = -1.5f; i <= 1.5f; i += 1.0f) {
//        glPushMatrix();
//        glTranslatef(i * 0.07f, 0.0f, 0.0f);   // X축으로 밀고 아래로 살짝 내림
//        glRotatef(90, 0, 0, 1);                // Z축 기준으로 회전
//        glScalef(4.0, 1.0, 1.0);
//        glutSolidCylinder(0.03, 0.25, 10, 5);   // 손가락
//
//        glPushMatrix();
//        drawHalfSphere(1.0,0,32);
//        glPopMatrix();
//
//        glPopMatrix();
//    }
//
//    glPopMatrix();
//}

// 배아랫 부분 ----------------------------------------------------------------------
void legPart() {
    //glColor3f(0.4, 0.6, 0.9); // 바지색

    //다리.(배, 엉덩이)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glPushMatrix();
    glScalef(1.1, 1, 1.1);
    drawHalfSphere(0.35, 30, 10);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //다리.(오른쪽)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    //glColor3f(0.3, 0.3, 0.3);
    glPushMatrix();
    glTranslatef(-0.15, -0.8, 0.0);
    glScalef(0.9, 1.1, 1.15);
    drawCylinder(0.15, 0.7, 7);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //다리.(왼쪽)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    //glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(0.15, -0.8, 0.0);
    glScalef(0.9, 1.1, 1.15);
    drawCylinder(0.15, 0.7, 7);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);


    //신발 부분 (왼쪽)
    glColor3f(0.2, 0.2, 0.2); // 색깔

    //신발 전체 매트릭스
    glPushMatrix();
    glTranslatef(-0.15, -0.9, -0.1);
    glScalef(0.4, 0.2, 0.2);

    //구두굽
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.0);
    glScalef(1.1, 1.6, 1.0);
    drawingCube();
    glPopMatrix();

    //구두앞부분
    glPushMatrix();
    glRotatef(3.0, 1.0, 0.0, 0.0);
    glTranslatef(0, -0.1, 1);
    glScalef(0.28, 1, 1);
    //구두 앞 깔창 반원
    glPushMatrix();
    drawHalfCircle(1.13, 0, 32);
    glPopMatrix();
    //깔창
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.1);
    glScalef(1.07, 1.0, 0.65);
    glBegin(GL_QUADS);
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-1.0, 0.0, -1.0);
    glVertex3f(1.0, 0.0, -1.0);
    glVertex3f(1.0, 0.0, 1.0);
    glVertex3f(-1.0, 0.0, 1.0);
    glEnd();
    glPopMatrix();
    //구두 윗부분
    glPushMatrix();
    glTranslatef(0.0, -0.05, -0.3);
    glRotatef(90, 0.0, 0.0, 1.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    glRotatef(90, 0.0, 0.0, 1.0);
    glScalef(2.2, 1.7, 1.5);
    drawHalfCapsule(0.5, 0.7, 32, 16);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    //신발 부분 (오른쪽) 
    glColor3f(0.2, 0.2, 0.2); // 색깔

    //신발 전체 매트릭스
    glPushMatrix();
    glTranslatef(+0.15, -0.9, -0.1);
    glScalef(0.4, 0.2, 0.2);

    //구두굽
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.0);
    glScalef(1.1, 1.6, 1.0);
    drawingCube();
    glPopMatrix();

    //구두앞부분
    glPushMatrix();
    glRotatef(3.0, 1.0, 0.0, 0.0);
    glTranslatef(0, -0.1, 1);
    glScalef(0.28, 1, 1);
    //구두 앞 깔창 반원
    glPushMatrix();
    drawHalfCircle(1.13, 0, 32);
    glPopMatrix();
    //깔창
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.1);
    glScalef(1.07, 1.0, 0.65);
    glBegin(GL_QUADS);
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-1.0, 0.0, -1.0);
    glVertex3f(1.0, 0.0, -1.0);
    glVertex3f(1.0, 0.0, 1.0);
    glVertex3f(-1.0, 0.0, 1.0);
    glEnd();
    glPopMatrix();
    //구두 윗부분
    glPushMatrix();
    glTranslatef(0.0, -0.05, -0.3);
    glRotatef(90, 0.0, 0.0, 1.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    glRotatef(90, 0.0, 0.0, 1.0);
    glScalef(2.2, 1.7, 1.5);
    drawHalfCapsule(0.5, 0.7, 32, 16);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();
}

// Half Sphere --------------------------------------------------------------------
void drawHalfSphere(float radius, int slices, int stacks) {
    for (int i = 0; i <= stacks / 2; ++i) {
        float lat0 = PI * (-0.5 + (float)i / stacks);
        float lat1 = PI * (-0.5 + (float)(i + 1) / stacks);
        float y0 = sinf(lat0), y1 = sinf(lat1);
        float r0 = cosf(lat0), r1 = cosf(lat1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2 * PI * j / slices;
            float x = cosf(lng), z = sinf(lng);

            // UV 좌표 계산
            float u = (float)j / slices;
            float v0 = (float)i / stacks;
            float v1 = (float)(i + 1) / stacks;

            glNormal3f(x * r0, y0, z * r0);
            glTexCoord2f(u, v0); // uv좌표 기입. 
            glVertex3f(radius * x * r0, radius * y0, radius * z * r0);

            glNormal3f(x * r1, y1, z * r1);
            glTexCoord2f(u, v1); // uv좌표 기입.              
            glVertex3f(radius * x * r1, radius * y1, radius * z * r1);
        }
        glEnd();
    }
}

// Cylinder ------------------------------------------------------------------------
void drawCylinder(float radius, float height, int segments) {
    float angleStep = 2.0 * PI / segments;

    // 옆면
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = cosf(angle), z = sinf(angle);
        float u = (float)i / segments;

        glNormal3f(x, 0.0, z);

        // 아래 vertex (v = 0.0)
        glTexCoord2f(u, 0.0);
        glVertex3f(radius * x, 0.0, radius * z);

        // 위 vetex (v = 1.0)
        glTexCoord2f(u, 1.0);
        glVertex3f(radius * x, height, radius * z);
    }
    glEnd();

    // 아래 바닥
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0, -1.0, 0.0);
    // 중앙점에 (0.5,0.5)
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0.0, 0.0, 0.0);

    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = cosf(angle), z = sinf(angle);
        // 원판에 (u,v) = (0.5 + 0.5*x, 0.5 + 0.5*z)로 계산하게 한다. 
        float u = 0.5 + 0.5 * x;
        float v = 0.5 + 0.5 * z;
        glTexCoord2f(u, v);
        glVertex3f(radius * x, 0.0, radius * z);
    }
    glEnd();

    // 위 바닥
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0, 1.0, 0.0);
    // 중앙점
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0.0, height, 0.0);

    for (int i = 0; i <= segments; ++i) {
        float angle = -i * angleStep;  // 시계 반대 방향
        float x = cosf(angle), z = sinf(angle);
        float u = 0.5 + 0.5 * x;
        float v = 0.5 + 0.5 * z;
        glTexCoord2f(u, v);
        glVertex3f(radius * x, height, radius * z);
    }
    glEnd();
}

// Half_Capsule ----------------------------------------------------------------------
void drawHalfCapsule(float radius, float height, int slices, int stacks) {

    float halfHeight = height * 0.5;

    // 1) 실린더 반쪽 곡면
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= slices; ++j) {
        float theta = PI * j / slices;
        float cx = cosf(theta), cz = sinf(theta);
        glNormal3f(cx, 0, cz);
        glVertex3f(radius * cx, -halfHeight, radius * cz);
        glVertex3f(radius * cx, halfHeight, radius * cz);
    }
    glEnd();

    // 2) 위쪽 반구 반쪽
    for (int i = 0; i < stacks; ++i) {
        float phi0 = (PI / 2) * i / stacks;
        float phi1 = (PI / 2) * (i + 1) / stacks;
        float y0 = sinf(phi0), y1 = sinf(phi1);
        float r0 = cosf(phi0), r1 = cosf(phi1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = PI * j / slices;
            float cx = cosf(theta), cz = sinf(theta);

            // 아래 줄 (접점)
            glNormal3f(r0 * cx, y0, r0 * cz);
            glVertex3f(radius * r0 * cx, halfHeight + radius * y0, radius * r0 * cz);

            // 위 줄
            glNormal3f(r1 * cx, y1, r1 * cz);
            glVertex3f(radius * r1 * cx, halfHeight + radius * y1, radius * r1 * cz);
        }
        glEnd();
    }

    // 3) 바닥 캡 (원판)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    // 중앙점
    glVertex3f(0, -halfHeight, 0);

    // 
    for (int j = 0; j <= slices; ++j) {
        float theta = PI * j / slices;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        glVertex3f(x, -halfHeight, z);
    }
    glEnd();

    // 실린더 단면
    glBegin(GL_QUAD_STRIP);
    glNormal3f(-1, 0, 0);
    glVertex3f(0, -halfHeight, 0);
    glVertex3f(0, halfHeight, 0);
    glVertex3f(0, -halfHeight, radius);
    glVertex3f(0, halfHeight, radius);
    glEnd();

    // 원판 단면
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-1, 0, 0);
    glVertex3f(0, -halfHeight, 0);
    for (int j = 0; j <= stacks; ++j) {
        float phi = (PI / 2) * (stacks - j) / stacks;
        float y = -halfHeight + radius * sinf(phi);
        float r = radius * cosf(phi);
        glVertex3f(0, y, r);
    }
    glEnd();
}

// HalfCircle -------------------------------------------------------
void drawHalfCircle(float radius, float y, int slices) {
    glNormal3f(0.0, 1.0, 0.0);

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float theta = PI * i / slices;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);

        glVertex3f(0.0, y, 0.0);
        glVertex3f(x, y, z);
    }
    glEnd();
}
//Cube ----------------------------------------------------------------
void drawingCube() {
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-0.25, -0.25, 0.25);
    glVertex3f(0.25, -0.25, 0.25);
    glVertex3f(0.25, 0.25, 0.25);
    glVertex3f(-0.25, 0.25, 0.25);

    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(0.25, -0.25, -0.25);
    glVertex3f(-0.25, -0.25, -0.25);
    glVertex3f(-0.25, 0.25, -0.25);
    glVertex3f(0.25, 0.25, -0.25);

    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-0.25, -0.25, -0.25);
    glVertex3f(-0.25, -0.25, 0.25);
    glVertex3f(-0.25, 0.25, 0.25);
    glVertex3f(-0.25, 0.25, -0.25);

    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(0.25, -0.25, 0.25);
    glVertex3f(0.25, -0.25, -0.25);
    glVertex3f(0.25, 0.25, -0.25);
    glVertex3f(0.25, 0.25, 0.25);

    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-0.25, 0.25, 0.25);
    glVertex3f(0.25, 0.25, 0.25);
    glVertex3f(0.25, 0.25, -0.25);
    glVertex3f(-0.25, 0.25, -0.25);

    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-0.25, -0.25, -0.25);
    glVertex3f(0.25, -0.25, -0.25);
    glVertex3f(0.25, -0.25, 0.25);
    glVertex3f(-0.25, -0.25, 0.25);
    glEnd();
}



// 소매
void drawArm(bool isLeft) {

    glPushMatrix();
    glColor3f(0.94f, 0.939f, 0.94f); // 흰색

    float xOffset = isLeft ? -0.55f : 0.55f;
    float baseRotation = isLeft ? -90.0f : 90.0f;

    /*glRotatef(50.0f, 0.0f, 0.0f, 0.0f);*/
    glTranslatef(xOffset, 0.35f, 0.0f);       // 초기 위치 조정
    glRotatef(baseRotation, 0, 1, 0);         // 좌우 정렬

    int steps = 15;
    float angleStep = 5.0f;
    float segmentLength = 0.15f;
    float accumulatedAngle = 0.0f;

    for (int i = 0; i < steps; ++i) {
        glPushMatrix();

        if (isLeft) {
            // 왼쪽: x축 기준 정방향 회전
            glRotatef(accumulatedAngle, 1, 0, 0);
            accumulatedAngle += angleStep;
        }
        else {
            // 오른쪽: x축 기준 역방향 회전
            glRotatef(-accumulatedAngle, 1, 0, 0);
            accumulatedAngle -= angleStep;
        }

        glTranslatef(0.0f, segmentLength, 0.0f);
        glutSolidTorus(0.05, 0.15, 20, 40);

        glPopMatrix();
    }
    // 팔 추가 (토러스 마지막 위치 기준)
    glPushMatrix();

    float finalAngle = isLeft ? accumulatedAngle : -accumulatedAngle;
    glRotatef(finalAngle, 1, 0, 0);           // 마지막 토러스와 동일한 회전
    glTranslatef(0.0f, segmentLength, 0.0f);  // 마지막 위치로 이동

    glColor3f(0.967f, 0.791f, 0.056f);              // 노란색
    glutSolidCylinder(0.16, 0.6, 20, 10);     // 팔 (Y축 방향)

    // 팔 끝에서 손 추가
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.6f);           // 팔 길이만큼 이동
    drawHand(isLeft);                         // 손 함수 호출
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();
}

void drawSleeve(bool isLeft) {
    drawArm(isLeft);
}

void drawFullArm(bool isLeft) {
    drawSleeve(isLeft);

}

// ------------------- Head -------------------
void drawHead() {
    float neckExtension = 0.3f;

    // [1] 목 포함 기둥
    glPushMatrix();
    glTranslatef(0.0f, -neckExtension, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glColor3f(0.967f, 0.791f, 0.056f);
    gluCylinder(quad, head_radius, head_radius, head_height + neckExtension, 40, 40);
    gluDisk(quad, 0.0f, head_radius, 40, 1);  // 밑바닥
    glPopMatrix();

    // [2] 머리 위 반구
    glPushMatrix();
    glTranslatef(0.0f, head_height, 0.0f);  // 기존 위치 그대로 유지
    glColor3f(0.967f, 0.791f, 0.056f);
    glutSolidSphere(head_radius, 40, 40);
    glPopMatrix();
}

void drawPupilSphere(float radius) {
    glutSolidSphere(radius, 16, 16); // 간단한 구체
}

// ------------------- Eyes -------------------
void drawEyes() {
    float eyey = 0.52f;
    float eyez = 0.38f;
    float eye_offset_x = 0.125f;

    float pupil_radius = 0.025f;
    float eye_radius = 0.18f;
    float pupil_offset_z = eye_radius * 0.95f; // 눈 표면 가까이
    float pupil_offset_y = 0.015f;

    // 왼쪽 눈
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glPushMatrix();
    glTranslatef(-eye_offset_x, eyey, eyez);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(-50.0f, 0.0f, 1.0f, 0.0f);
    drawSphere(eye_radius, 30, 30);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // 왼쪽 동공
    glPushMatrix();
    glTranslatef(-eye_offset_x - 0.07f, eyey + pupil_offset_y, eyez + pupil_offset_z);
    glRotatef(-50.0f, 0.0f, 1.0f, 0.0f); // 눈과 동일하게 회전
    glColor3f(0.0f, 0.0f, 0.0f);
    glutSolidSphere(pupil_radius, 20, 20);
    glPopMatrix();

    // 오른쪽 눈
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glPushMatrix();
    glTranslatef(eye_offset_x, eyey, eyez);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(116.0f, 0.0f, 1.0f, 0.0f);
    drawSphere(eye_radius, 30, 30);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // 오른쪽 동공
    glPushMatrix();
    glTranslatef(eye_offset_x + 0.05f, eyey + pupil_offset_y, eyez + pupil_offset_z);
    glRotatef(116.0f, 0.0f, 1.0f, 0.0f); // 눈과 동일하게 회전
    glColor3f(0.0f, 0.0f, 0.0f);
    glutSolidSphere(pupil_radius, 20, 20);
    glPopMatrix();
}


// ------------------- Nose -------------------
void drawNose() {
    float nose_radius = 0.05f;
    float nose_length = 0.12f;
    float base_y = 0.42f;

    glPushMatrix();
    glTranslatef(0.0f, base_y, 0.45f);
    glColor3f(0.967f, 0.791f, 0.056f);
    gluCylinder(quad, nose_radius, nose_radius, nose_length, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, base_y, 0.45f + nose_length);
    glColor3f(0.967f, 0.791f, 0.056f);
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

    float baseY = head_height + head_radius - 0.16f;

    glBegin(GL_LINE_STRIP);
    for (float t = 0; t <= PI; t += 0.05f) {
        float x = -0.01f + 0.3f * cos(t);
        float y = baseY + 0.22f * sin(t);
        float z = -0.07f;
        glVertex3f(x, y, z);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (float t = 0; t <= PI; t += 0.05f) {
        float x = 0.01f + 0.3f * cos(t);
        float y = baseY + 0.22f * sin(t);
        float z = 0.03f;
        glVertex3f(x, y, z);
    }
    glEnd();

    glPopMatrix();
}

// ------------------- Half Torus -------------------
void drawHalfTorus(float outerRadius, float innerRadius, int sides, int rings) {
    float twoPi = 2.0f * PI;
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
    glColor3f(0.967f, 0.791f, 0.056f);
    drawHalfTorus(0.1f, 0.02f, 20, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(ear_offset_x, ear_y, ear_z);
    glRotatef(180, 1, 1, 0);
    glColor3f(0.967f, 0.791f, 0.056f);
    drawHalfTorus(0.1f, 0.02f, 20, 30);
    glPopMatrix();
}


// Sphere--------------------------------------------------------
void drawSphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; ++i) {
        float lat0 = PI * (-0.5f + (float)i / stacks);
        float lat1 = PI * (-0.5f + (float)(i + 1) / stacks);
        float y0 = sinf(lat0), y1 = sinf(lat1);
        float r0 = cosf(lat0), r1 = cosf(lat1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2 * PI * (float)j / slices;
            float x = cosf(lng), z = sinf(lng);



            float u = ((float)j / slices);
            float v0 = (asinf(y0) / PI) + 0.54f;  // 왜곡 보정
            float v1 = (asinf(y1) / PI) + 0.54f;

            glNormal3f(x * r0, y0, z * r0);
            glTexCoord2f(u, v0);
            glVertex3f(radius * x * r0, radius * y0, radius * z * r0);

            glNormal3f(x * r1, y1, z * r1);
            glTexCoord2f(u, v1);
            glVertex3f(radius * x * r1, radius * y1, radius * z * r1);
        }
        glEnd();
    }
}


// DIsplay ----------------------------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCameraSpace();

    glPushMatrix();
    glRotatef(90, 0, 1, 0);  // 정면 방향 돌리기
    drawHomerTorso();
    glPopMatrix();

    // 양쪽 팔
    glPushMatrix();
    glTranslated(0.0f, -0.46f, 0.0f);
    glRotatef(-15.0f, 0.0, 0.0, 1.0);
    drawFullArm(true);  // 왼팔
    glPopMatrix();
    glPushMatrix();
    glTranslated(0.0f, -0.46f, 0.0f);
    glRotatef(15.0f, 0.0, 0.0, 1.0);
    drawFullArm(false); // 오른팔
    glPopMatrix();

    //다리
    glPushMatrix();
    glTranslated(0.0f, -0.4f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);
    legPart();
    glPopMatrix();

    // 머리
    glPushMatrix();
    glTranslated(0.0, 0.5, 0.06);
    glScalef(0.6f, 0.6f, 0.6f);
    drawHead();
    drawEyes();
    drawNose();
    drawMouthArea();
    drawHairLines();
    drawEars();
    glPopMatrix();

    glutSwapBuffers();
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

//마우스 이벤트 (좌클릭시) --------------------------------------------------------
void inputMouse(int button, int state, int x, int y)
{

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseX = x;
        mouseY = y;
        isDragging = true;  // 드래그 시작
    }
    else isDragging = false; // 드래그 종료
}

//마우스 드래그 이벤트 -------------------------------------------------------------
void dragMouse(int x, int y) {
    if (isDragging) {
        float deltaX = (x - mouseX) * 0.005;
        float deltaY = (y - mouseY) * 0.005;

        cameraTheta += deltaX;
        cameraPhi += deltaY;

        // phi 제한 (0.01 ~ pi - 0.01 사이)
        if (cameraPhi < 0.01) cameraPhi = 0.01;
        if (cameraPhi > 3.13) cameraPhi = 3.13;

        mouseX = x;
        mouseY = y;

        glutPostRedisplay();
    }
}

// 마우스 휠 줌인 줌아웃
void mouseWheel(int button, int dir, int x, int y) {
    if (dir > 0) {
        cameraRadius -= 0.5;
        if (cameraRadius < 0.05) cameraRadius = 0.05;
    }
    else {
        cameraRadius += 0.5;
    }
    glutPostRedisplay();
}


//키보드 줌 인 (a) / 줌 아웃 (z) --------------------------------------------------
void inputKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'a':  // 줌인
        cameraRadius -= 0.1;
        if (cameraRadius < 0.05) cameraRadius = 0.05; // 최소 거리 제한
        glutPostRedisplay();
        std::cout << "줌인" << std::endl;
        break;
    case 'z':  // 줌아웃
        cameraRadius += 0.1;
        glutPostRedisplay();
        std::cout << "줌아웃" << std::endl;
        break;
    }
}

void init() {
    //컬링 및 뎁스, 페이스 방향 설정
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //텍스쳐 설정
    glEnable(GL_TEXTURE_2D);

    //텍스쳐 버퍼 생성
    glGenTextures(3, textures);

    //텍스쳐 로드 함수로 텍스쳐 로드
    loadTexture("shirts.png", textures[0]);
    loadTexture("denim.png", textures[1]);
    /*loadTexture("eye.png", textures[2]);*/

    // 텍스처가 컬러에 섞이지 않고 순수하게 보여지도록 한다.
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluQuadricTexture(quad, GL_TRUE);

    glDisable(GL_TEXTURE_2D);
}

// main 함수 ----------------------------------------------------------------------
int main(int argc, char** argv) {
    //기본 함수
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Homer Simpson");

    /*initLighting();*/
    init();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    //마우스 입력 && 드래깅
    glutMouseFunc(inputMouse);
    glutMotionFunc(dragMouse);

    // 마우스 줌인 줌아웃
    glutMouseWheelFunc(mouseWheel);

    //키보드 입력 ( 줌 인 / 줌 아웃)
    glutKeyboardFunc(inputKeyboard);

    glClearColor(0.529, 0.808, 0.922, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    PlaySound(TEXT("SimpsonBGM.wav"), NULL, SND_ASYNC | SND_LOOP);
    glutMainLoop();


}