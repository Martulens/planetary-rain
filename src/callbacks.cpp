#include "framework.h"
#include "callbacks.h"

#include <imgui_impl_glut.h>
#include <scene.h>

#include "config.h"
#include "draw.h"
#include "game.h"
#include "utilities.h"

#include <GL/freeglut_ext.h>
#include <GL/freeglut_std.h>

#include "camera.h"
#include "context.h"
#include "gamestate.h"
#include "ui.h"

static PlanetParams g_planetParams;
static bool g_paramsChanged = false;

void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY) {
    // Forward to ImGui first if in UI mode
    if (con::gameState->uiMode) {
        uiKeyboardCallback(keyPressed, mouseX, mouseY);
    }

    switch (keyPressed) {
    case 27: // ESC
        glutLeaveMainLoop();
        break;
    case 'r':
        restartGame();
        break;
    case '\t':
        con::gameState->uiMode = !con::gameState->uiMode;
        if (con::gameState->uiMode) {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        } else {
            glutSetCursor(GLUT_CURSOR_NONE);
            // Re-center mouse when returning to FPS mode
            int centerX = con::gameState->windowWidth / 2;
            int centerY = con::gameState->windowHeight / 2;
            glutWarpPointer(centerX, centerY);
        }
        break;

    case 'w':
    case 'W':
        if (!con::gameState->uiMode) con::gameState->keyW = true;
        break;
    case 's':
    case 'S':
        if (!con::gameState->uiMode) con::gameState->keyS = true;
        break;
    case 'a':
    case 'A':
        if (!con::gameState->uiMode) con::gameState->keyA = true;
        break;
    case 'd':
    case 'D':
        if (!con::gameState->uiMode) con::gameState->keyD = true;
        break;
    case ' ':
        if (!con::gameState->uiMode) con::gameState->keySpace = true;
        break;
    case 'f':
    case 'F':
        con::gameState->wireframeMode = !con::gameState->wireframeMode;
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

void onMouseClick(int button, int state, int x, int y) {
    if (con::gameState->uiMode) {
        uiMouseCallback(button, state, x, y);
    }
}

void onMouseMove(int x, int y) {
    // UI mode: forward to ImGui, no camera movement
    if (con::gameState->uiMode) {
        uiMotionCallback(x, y);
        return;
    }

    // FPS mode: your existing camera code
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

void onReshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    con::gameState->windowWidth = w;
    con::gameState->windowHeight = h;
    con::camera->update(w, h);
    uiReshapeCallback(w, h);
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

void onDisplay(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateMovement();

    CHECK_GL_ERROR();
    drawWindow();

    renderUI(g_planetParams, g_paramsChanged);

    if (g_paramsChanged && con::scene) {
        if (g_planetParams.changed) {
            con::scene->updatePlanet(g_planetParams);
        }
    }

    CHECK_GL_ERROR();
    glutSwapBuffers();

    FPS();
}