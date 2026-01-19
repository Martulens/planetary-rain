#include "framework.h"
#include "callbacks.h"

#include "config.h"
#include "draw.h"
#include "game.h"
#include "utilities.h"

#include <GL/freeglut_ext.h>
#include <GL/freeglut_std.h>

#include "context.h"

void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY)
{
    switch (keyPressed)
    {
    case 27:
        glutLeaveMainLoop();
        break;
    case 'o':
        con::camera->pitch = 30.0f;
        break;
    case 'p':
        con::camera->distance = 20.0f;
    case 'r':
        restartGame();
        break;
    }
}

void onMouseClick(int button, int state, int x, int y)
{
    return;
}

void onMouseMove(int x, int y)
{
    int centerX = con::gameState->windowWidth / 2;
    int centerY = con::gameState->windowHeight / 2;

    int xOffset = x - centerX;
    int yOffset = y - centerY;
    con::gameState->lastMouseX = xOffset;
    con::gameState->lastMouseY = yOffset;

    float dx = (float)(x - centerX);
    float dy = (float)(centerY - y);

    dx *= PLAYER_MOUSE_SENSITIVITY;
    dy *= PLAYER_MOUSE_SENSITIVITY;

    glm::vec3 lookDir;
    con::camera->viewAngle -= dx;
    con::camera->pitch += dy;

    con::camera->pitch = glm::clamp(con::camera->pitch, -89.0f, 89.0f);

    float yaw = glm::radians(con::camera->viewAngle);
    float pitch = glm::radians(con::camera->pitch);

    lookDir.x = cos(pitch) * -sin(yaw);
    lookDir.y = sin(pitch);
    lookDir.z = -cos(pitch) * cos(yaw);

    con::camera->update( -1, -1);
    con::camera->changeDir(glm::normalize(lookDir));

    glutWarpPointer(centerX, centerY);
}

void onMouseScroll(int button, int dir, int x, int y){
    con::camera->zoom(dir * 2.0f);
}

void onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CHECK_GL_ERROR();
    drawWindow();

    CHECK_GL_ERROR();
    glutSwapBuffers();
    glutPostRedisplay();

    FPS();
}

void onReshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void onSpecialKeyPress(int key, int x, int y)
{
    return;
}

void onSpecialKeyRelease(int key, int x, int y)
{
    return;
}


void timer(int)
{
    glutPostRedisplay();
    glutTimerFunc(refreshTimeMs, timer, 0);
}
