#include "draw.h"

#include "framework.h"
#include "gamestate.h"

void drawSphere(glm::vec3 position, float radius){
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    glutWireSphere(radius, 16, 16);
    // Or use solid: glutSolidSphere(radius, 16, 16);

    glPopMatrix();
}


void drawCube(glm::vec3 position, float size){
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    // Wire cube for debugging
    glutWireCube(size);

    // Or use solid: glutSolidCube(size);

    glPopMatrix();
}


void drawWindow() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    if (h == 0)
        h = 1;


    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    CHECK_GL_ERROR();

    // === DRAWING
    glColor3f(1.0f, 0.0f, 0.0f);  // Red
    drawSphere(glm::vec3(-3.0f, 1.0f, -1.0f), 1.0f);

    glColor3f(0.0f, 1.0f, 0.0f);  // Green
    drawCube(glm::vec3(3.0f, 1.0f, -1.0f), 2.0f);

    // edges
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // fill
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glViewport(0, 0, w, h);

    CHECK_GL_ERROR();
}
