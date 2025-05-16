#include <glut.h>
#include <freeglut.h>
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

void drawHand(bool isLeft) {
    glPushMatrix();

    // 손바닥
    glColor3f(1.0f, 1.0f, 0.0f); // 노란색
    glutSolidSphere(0.16, 20, 20); // 손바닥

    // 손가락 4개 (중앙 정렬, 균형 잡힌 간격)
    for (float i = -1.5f; i <= 1.5f; i += 1.0f) {
        glPushMatrix();
        glTranslatef(i * 0.07f, 0.0f, 0.0f);   // X축으로 밀고 아래로 살짝 내림
        glRotatef(90, 0, 0, 1);                // Z축 기준으로 회전
        glutSolidCylinder(0.03, 0.25, 10, 5);   // 손가락
        glPopMatrix();
    }

    glPopMatrix();
}


// 소매
void drawArm(bool isLeft) {

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f); // 흰색

    float xOffset = isLeft ? -0.55f : 0.55f;
    float baseRotation = isLeft ? -90.0f : 90.0f;

    glTranslatef(xOffset, 0.35f, 0.0f);       // 초기 위치 조정
    glRotatef(baseRotation, 0, 1, 0);         // 좌우 정렬

    int steps = 15;
    float angleStep = 6.0f;
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

    glColor3f(1.0f, 1.0f, 0.0f);              // 노란색
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

// DIsplay ----------------------------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCameraSpace();

    // 양쪽 팔
    drawFullArm(true);  // 왼팔
    drawFullArm(false); // 오른팔

    glutSwapBuffers();
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

// main 함수 ----------------------------------------------------------------------
int main(int argc, char** argv) {
    //기본 함수
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Homer Simpson");

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

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutMainLoop();
}