#include "debug.h"
#include <GL/glew.h>
#include <GL/freeglut_std.h>


void drawSphere(glm::vec3 position, float radius) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glutSolidSphere(radius, 16, 16);
    glPopMatrix();
}

void drawCube(glm::vec3 position, float size) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glutWireCube(size);
    glPopMatrix();
}

void drawGrid() {
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_LINES);

    for (int i = -20; i <= 20; i++) {
        glVertex3f((float)i, 0.0f, -20.0f);
        glVertex3f((float)i, 0.0f, 20.0f);
        glVertex3f(-20.0f, 0.0f, (float)i);
        glVertex3f(20.0f, 0.0f, (float)i);
    }
    glEnd();
}