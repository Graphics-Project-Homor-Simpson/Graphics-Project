#include <glut.h>
#include <freeglut.h>
#include <cmath>
#include <iostream>

#define PI 3.141592

//���콺 ����
int mouseX, mouseY;

//���� ��ǥ�� Ȱ��
float cameraRadius = 5.0; // ���� �߽����� �ϴ� Sphere ������ ����
float cameraTheta = 1.0;
float cameraPhi = 1.57;

//�巡�� ����
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
    // ����Ʈ ���� 
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Perspective -> ���� ����
    gluPerspective(60.0, (float)w / h, 0.01, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

void drawHand(bool isLeft) {
    glPushMatrix();

    // �չٴ�
    glColor3f(1.0f, 1.0f, 0.0f); // �����
    glutSolidSphere(0.16, 20, 20); // �չٴ�

    // �հ��� 4�� (�߾� ����, ���� ���� ����)
    for (float i = -1.5f; i <= 1.5f; i += 1.0f) {
        glPushMatrix();
        glTranslatef(i * 0.07f, 0.0f, 0.0f);   // X������ �а� �Ʒ��� ��¦ ����
        glRotatef(90, 0, 0, 1);                // Z�� �������� ȸ��
        glutSolidCylinder(0.03, 0.25, 10, 5);   // �հ���
        glPopMatrix();
    }

    glPopMatrix();
}


// �Ҹ�
void drawArm(bool isLeft) {

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f); // ���

    float xOffset = isLeft ? -0.55f : 0.55f;
    float baseRotation = isLeft ? -90.0f : 90.0f;

    glTranslatef(xOffset, 0.35f, 0.0f);       // �ʱ� ��ġ ����
    glRotatef(baseRotation, 0, 1, 0);         // �¿� ����

    int steps = 15;
    float angleStep = 6.0f;
    float segmentLength = 0.15f;
    float accumulatedAngle = 0.0f;

    for (int i = 0; i < steps; ++i) {
        glPushMatrix();

        if (isLeft) {
            // ����: x�� ���� ������ ȸ��
            glRotatef(accumulatedAngle, 1, 0, 0);
            accumulatedAngle += angleStep;
        }
        else {
            // ������: x�� ���� ������ ȸ��
            glRotatef(-accumulatedAngle, 1, 0, 0);
            accumulatedAngle -= angleStep;
        }

        glTranslatef(0.0f, segmentLength, 0.0f);
        glutSolidTorus(0.05, 0.15, 20, 40);

        glPopMatrix();
    }
    // �� �߰� (�䷯�� ������ ��ġ ����)
    glPushMatrix();

    float finalAngle = isLeft ? accumulatedAngle : -accumulatedAngle;
    glRotatef(finalAngle, 1, 0, 0);           // ������ �䷯���� ������ ȸ��
    glTranslatef(0.0f, segmentLength, 0.0f);  // ������ ��ġ�� �̵�

    glColor3f(1.0f, 1.0f, 0.0f);              // �����
    glutSolidCylinder(0.16, 0.6, 20, 10);     // �� (Y�� ����)

    // �� ������ �� �߰�
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.6f);           // �� ���̸�ŭ �̵�
    drawHand(isLeft);                         // �� �Լ� ȣ��
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

    // ���� ��
    drawFullArm(true);  // ����
    drawFullArm(false); // ������

    glutSwapBuffers();
}

//���콺 �̺�Ʈ (��Ŭ����) --------------------------------------------------------
void inputMouse(int button, int state, int x, int y)
{

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseX = x;
        mouseY = y;
        isDragging = true;  // �巡�� ����
    }
    else isDragging = false; // �巡�� ����
}

//���콺 �巡�� �̺�Ʈ -------------------------------------------------------------
void dragMouse(int x, int y) {
    if (isDragging) {
        float deltaX = (x - mouseX) * 0.005;
        float deltaY = (y - mouseY) * 0.005;

        cameraTheta += deltaX;
        cameraPhi += deltaY;

        // phi ���� (0.01 ~ pi - 0.01 ����)
        if (cameraPhi < 0.01) cameraPhi = 0.01;
        if (cameraPhi > 3.13) cameraPhi = 3.13;

        mouseX = x;
        mouseY = y;

        glutPostRedisplay();
    }
}

// ���콺 �� ���� �ܾƿ�
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


//Ű���� �� �� (a) / �� �ƿ� (z) --------------------------------------------------
void inputKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'a':  // ����
        cameraRadius -= 0.1;
        if (cameraRadius < 0.05) cameraRadius = 0.05; // �ּ� �Ÿ� ����
        glutPostRedisplay();
        std::cout << "����" << std::endl;
        break;
    case 'z':  // �ܾƿ�
        cameraRadius += 0.1;
        glutPostRedisplay();
        std::cout << "�ܾƿ�" << std::endl;
        break;
    }
}

// main �Լ� ----------------------------------------------------------------------
int main(int argc, char** argv) {
    //�⺻ �Լ�
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

    //���콺 �Է� && �巡��
    glutMouseFunc(inputMouse);
    glutMotionFunc(dragMouse);
 
    // ���콺 ���� �ܾƿ�
    glutMouseWheelFunc(mouseWheel);

    //Ű���� �Է� ( �� �� / �� �ƿ�)'
    glutKeyboardFunc(inputKeyboard);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutMainLoop();
}