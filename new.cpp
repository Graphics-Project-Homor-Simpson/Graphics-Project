#include <glut.h>

void reshape(int w, int h) {
    // 뷰포트 설정 
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Perspective -> 원근 투영
    gluPerspective(60.0, 1.0, 1.0, 100.0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0, 0.0, -1.0,   // eye
        0.0, 0.0, 0.0,   // center
        0.0, 1.0, 0.0);  // up

    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    //기본 함수들
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Homer Simpson");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glLoadIdentity();
    glutMainLoop();
}
