#include <glut.h>
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
    // ����Ʈ ���� 
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Perspective -> ���� ����
    gluPerspective(60.0, (float)w / h, 0.01, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

// DIsplay ----------------------------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCameraSpace();

    //�⺻ �ﰢ��.
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-0.5, -0.5, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.5, 0.0);
    glEnd();

    //�ٸ�
    legPart();

    glutSwapBuffers();
}
// �ٸ��κ�


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

// �ٸ� �κ� ----------------------------------------------------------------------
void legPart() {
    glColor3f(0.4f, 0.6f, 0.9f); // ������
    drawCylinder(0.2, 0.5, 30);
}

// Cylinder ------------------------------------------------------------------------
void drawCylinder(float radius, float height, int segments) {
    float angleStep = 2.0f * PI / segments;

    // ��
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = cosf(angle);
        float z = sinf(angle);

        glNormal3f(x, 0.0f, z);  // ���� ����
        glVertex3f(radius * x, 0.0f, radius * z);       // �Ʒ� ��
        glVertex3f(radius * x, height, radius * z);     // �� ��
    }
    glEnd();

    // �Ʒ� �ٴ�
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        glVertex3f(radius * cosf(angle), 0.0f, radius * sinf(angle));
    }
    glEnd();

    // �� �ٴ�
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float angle = -i * angleStep;
        glVertex3f(radius * cosf(angle), height, radius * sinf(angle));
    }
    glEnd();
}

// main �Լ� ----------------------------------------------------------------------
int main(int argc, char** argv) {
    //�⺻ �Լ�
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Homer Simpson");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    //���콺 �Է� && �巡��
    glutMouseFunc(inputMouse);
    glutMotionFunc(dragMouse);

    //Ű���� �Է� ( �� �� / �� �ƿ�)'
    glutKeyboardFunc(inputKeyboard);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glLoadIdentity();
    glutMainLoop();
}