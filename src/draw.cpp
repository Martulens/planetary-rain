#include "draw.h"

#include "framework.h"
#include "gamestate.h"

void drawSphere(glm::vec3 position, float radius){

}


void drawCube(glm::vec3 position, float radius);


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

    // edges
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // fill
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glViewport(0, 0, w, h);

    CHECK_GL_ERROR();
}
