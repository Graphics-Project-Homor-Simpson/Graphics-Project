#include <glut.h>
#include <cmath>
#include <iostream>

#define PI 3.141592

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
void drawCylinder(float radius, float height, int segments);

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

// DIsplay ----------------------------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCameraSpace();

    //기본 삼각형.
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-0.5, -0.5, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.5, 0.0);
    glEnd();

    //다리
    legPart();

    glutSwapBuffers();
}
// 다리부분


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

// 다리 부분 ----------------------------------------------------------------------
void legPart() {
    glColor3f(0.4f, 0.6f, 0.9f); // 바지색
    drawCylinder(0.2, 0.5, 30);
}

// Cylinder ------------------------------------------------------------------------
void drawCylinder(float radius, float height, int segments) {
    float angleStep = 2.0f * PI / segments;

    // 옆
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = cosf(angle);
        float z = sinf(angle);

        glNormal3f(x, 0.0f, z);  // 옆면 법선
        glVertex3f(radius * x, 0.0f, radius * z);       // 아래 원
        glVertex3f(radius * x, height, radius * z);     // 위 원
    }
    glEnd();

    // 아래 바닥
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        glVertex3f(radius * cosf(angle), 0.0f, radius * sinf(angle));
    }
    glEnd();

    // 위 바닥
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float angle = -i * angleStep;
        glVertex3f(radius * cosf(angle), height, radius * sinf(angle));
    }
    glEnd();
}

// main 함수 ----------------------------------------------------------------------
int main(int argc, char** argv) {
    //기본 함수
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Homer Simpson");
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