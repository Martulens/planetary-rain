#include "framework.h"
#include "callbacks.h"

#include "config.h"
#include "draw.h"
#include "game.h"
#include "utilities.h"

#include <GL/freeglut_ext.h>
#include <GL/freeglut_std.h>

#include "camera.h"
#include "context.h"
#include "gamestate.h"

void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY){
    switch (keyPressed)
    {
    case 27:
        glutLeaveMainLoop();
        break;
    case 'r':
        restartGame();
        break;
    case 'w':
    case 'W':
        con::gameState->keyW = true;
        break;
    case 's':
    case 'S':
        con::gameState->keyS = true;
        break;
    case 'a':
    case 'A':
        con::gameState->keyA = true;
        break;
    case 'd':
    case 'D':
        con::gameState->keyD = true;
        break;
    case ' ':
        con::gameState->keySpace = true;
        break;
    // Toggle wireframe mode with F key
    case 'f':
    case 'F':
        con::gameState->wireframeMode = !con::gameState->wireframeMode;
        std::cout << "Wireframe mode: " << (con::gameState->wireframeMode ? "ON" : "OFF") << std::endl;
        break;
    }
}

void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY){
    switch (keyReleased)
    {
    case 'w':
    case 'W':
        con::gameState->keyW = false;
        break;
    case 's':
    case 'S':
        con::gameState->keyS = false;
        break;
    case 'a':
    case 'A':
        con::gameState->keyA = false;
        break;
    case 'd':
    case 'D':
        con::gameState->keyD = false;
        break;
    case ' ':
        con::gameState->keySpace = false;
        break;
    }
}

void onMouseClick(int button, int state, int x, int y){
    return;
}

void onMouseMove(int x, int y){
    static bool warping = false;
    if (warping) {
        warping = false;
        return;
    }

    int centerX = con::gameState->windowWidth / 2;
    int centerY = con::gameState->windowHeight / 2;

    int dx = x - centerX;
    int dy = centerY - y;

    if (dx == 0 && dy == 0) return;

    con::camera->viewAngle -= dx * PLAYER_MOUSE_SENSITIVITY;
    con::camera->pitch += dy * PLAYER_MOUSE_SENSITIVITY;
    con::camera->pitch = glm::clamp(con::camera->pitch, -89.0f, 89.0f);

    con::camera->update(-1, -1);

    warping = true;
    glutWarpPointer(centerX, centerY);
}

void onMouseScroll(int button, int dir, int x, int y) {
    con::camera->zoom(dir * 2.0f);
}

void updateMovement() {
    if (con::gameState->keyW)
        con::camera->moveForward(MOVE_SPEED);
    if (con::gameState->keyS)
        con::camera->moveForward(-MOVE_SPEED);
    if (con::gameState->keyA)
        con::camera->moveRight(-MOVE_SPEED);
    if (con::gameState->keyD)
        con::camera->moveRight(MOVE_SPEED);
    if (con::gameState->keySpace)
        con::camera->moveUp(MOVE_SPEED);

    con::camera->update(-1, -1);
}

void onDisplay(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateMovement();

    CHECK_GL_ERROR();
    drawWindow();

    CHECK_GL_ERROR();
    glutSwapBuffers();

    FPS();
}

void onReshape(int w, int h){
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    con::gameState->windowWidth = w;
    con::gameState->windowHeight = h;
    con::camera->update(w, h);
}

void onSpecialKeyPress(int key, int x, int y){
    switch (key) {
    case GLUT_KEY_SHIFT_L:
    case GLUT_KEY_SHIFT_R:
        con::gameState->keyShift = true;
        break;
    }
}

void onSpecialKeyRelease(int key, int x, int y){
    switch (key) {
    case GLUT_KEY_SHIFT_L:
    case GLUT_KEY_SHIFT_R:
        con::gameState->keyShift = false;
        break;
    }
}

void timer(int){
    glutPostRedisplay();
    glutTimerFunc(refreshTimeMs, timer, 0);
}