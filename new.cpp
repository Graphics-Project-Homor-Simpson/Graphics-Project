#include <glut.h>
#include <cmath>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define PI 3.141592
#define NUM_TEXTURES 1

//텍스쳐 변수
GLuint textures[NUM_TEXTURES];

//마우스 변수
int mouseX, mouseY;

//구면 좌표계 활용
float cameraRadius = 5.0; // 원점 중심으로 하는 Sphere 반지름 설정
float cameraTheta = 1.0;
float cameraPhi = 1.57;

//드래깅 여부
bool isDragging;

// Function 
void legPart();
void drawHalfSphere(float radius, int slices, int stacks);
void drawCylinder(float radius, float height, int segments);
void drawHalfCapsule(float radius, float height, int slices, int stacks);
void drawHalfCircle(float radius, float y, int slices);
void drawingCube();

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
    gluPerspective(60.0, (float)w / h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

// DIsplay ----------------------------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    updateCameraSpace();

    //다리
    legPart();
    glutSwapBuffers();

}


//마우스 이벤트 (좌클릭시) --------------------------------------------------------
void inputMouse(int button, int state, int x, int y) {
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

// 배아랫 부분 ----------------------------------------------------------------------
void legPart() {
    //glColor3f(0.4, 0.6, 0.9); // 바지색

    //다리.(배, 엉덩이)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glPushMatrix();
    glScalef(1.1, 1, 1.1);
    drawHalfSphere(0.3, 30, 10);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //다리.(오른쪽)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    //glColor3f(0.3, 0.3, 0.3);
    glPushMatrix();
    glTranslatef(-0.15, -0.8, 0.0);
    glScalef(0.9, 1.1, 1.15);
    drawCylinder(0.15, 0.7, 7);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //다리.(왼쪽)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
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

// 텍스쳐 로드 함수
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
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //텍스쳐 설정
    glEnable(GL_TEXTURE_2D);

    //텍스쳐 버퍼 생성
    glGenTextures(1, textures);

    //텍스쳐 로드 함수로 텍스쳐 로드
    loadTexture("denim.png", textures[0]);

    // 텍스처가 컬러에 섞이지 않고 순수하게 보여지도록 한다.
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

// main 함수 ----------------------------------------------------------------------
int main(int argc, char** argv) {
    //기본 함수
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Homer Simpson");

    //init 함수 
    init();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    //마우스 입력 && 드래깅
    glutMouseFunc(inputMouse);
    glutMotionFunc(dragMouse);

    //키보드 입력 ( 줌 인 / 줌 아웃)'
    glutKeyboardFunc(inputKeyboard);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glLoadIdentity();
    glutMainLoop();
}