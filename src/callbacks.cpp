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
#include "gamestate.h"
#include "ui.h"
#include "variables.h"

void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY) {
    auto gameState = var::game->getGameState();
    
    // Forward to ImGui first if in UI mode
    if (gameState->uiMode) {
        uiKeyboardCallback(keyPressed, mouseX, mouseY);
    }

    switch (keyPressed) {
    case 27: // ESC
        glutLeaveMainLoop();
        break;
    case 'r':
        var::game->restartGame();
        break;
    case '\t':
        gameState->uiMode = !gameState->uiMode;
        if (gameState->uiMode) {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        } else {
            glutSetCursor(GLUT_CURSOR_NONE);
            // Re-center mouse when returning to FPS mode
            int centerX = gameState->windowWidth / 2;
            int centerY = gameState->windowHeight / 2;
            glutWarpPointer(centerX, centerY);
        }
        break;

    case 'w':
    case 'W':
        if (!gameState->uiMode) gameState->keyW = true;
        break;
    case 's':
    case 'S':
        if (!gameState->uiMode) gameState->keyS = true;
        break;
    case 'a':
    case 'A':
        if (!gameState->uiMode) gameState->keyA = true;
        break;
    case 'd':
    case 'D':
        if (!gameState->uiMode) gameState->keyD = true;
        break;
    case ' ':
        if (!gameState->uiMode) gameState->keySpace = true;
        break;
    case 'f':
    case 'F':
        gameState->wireframeMode = !gameState->wireframeMode;
        break;
    }
}

void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY){
    auto gameState = var::game->getGameState();

    switch (keyReleased){
    case 'w':
    case 'W':
        gameState->keyW = false;
        break;
    case 's':
    case 'S':
        gameState->keyS = false;
        break;
    case 'a':
    case 'A':
        gameState->keyA = false;
        break;
    case 'd':
    case 'D':
        gameState->keyD = false;
        break;
    case ' ':
        gameState->keySpace = false;
        break;
    }
}

void onMouseClick(int button, int state, int x, int y) {
    auto gameState = var::game->getGameState();

    if (gameState->uiMode) {
        uiMouseCallback(button, state, x, y);
    }
}

void onMouseMove(int x, int y) {
    auto gameState = var::game->getGameState();
    auto camera = var::game->getCamera();
    
    // UI mode: forward to ImGui, no camera movement
    if (gameState->uiMode) {
        uiMotionCallback(x, y);
        return;
    }

    // FPS mode: your existing camera code
    static bool warping = false;
    if (warping) {
        warping = false;
        return;
    }

    int centerX = gameState->windowWidth / 2;
    int centerY = gameState->windowHeight / 2;

    int dx = x - centerX;
    int dy = centerY - y;

    if (dx == 0 && dy == 0) return;

   camera->viewAngle -= dx * PLAYER_MOUSE_SENSITIVITY;
   camera->pitch += dy * PLAYER_MOUSE_SENSITIVITY;
   camera->pitch = glm::clamp(camera->pitch, -PITCH_VAL, PITCH_VAL);

   camera->update(-1, -1);

    warping = true;
    glutWarpPointer(centerX, centerY);
}

void onReshape(int w, int h) {
    auto gameState = var::game->getGameState();
    auto camera = var::game->getCamera();

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    gameState->windowWidth = w;
    gameState->windowHeight = h;
    camera->update(w, h);
    uiReshapeCallback(w, h);
}

void onMouseScroll(int button, int dir, int x, int y) {
    auto camera = var::game->getCamera();

    camera->zoom(dir * 2.0f);
}

void updateMovement() {
    auto gameState = var::game->getGameState();
    auto camera = var::game->getCamera();

    if (gameState->keyW)
       camera->moveForward(MOVE_SPEED);
    if (gameState->keyS)
       camera->moveForward(-MOVE_SPEED);
    if (gameState->keyA)
       camera->moveRight(-MOVE_SPEED);
    if (gameState->keyD)
       camera->moveRight(MOVE_SPEED);
    if (gameState->keySpace)
       camera->moveUp(MOVE_SPEED);

    if(!gameState->uiMode)
       camera->update(-1, -1);
}

void onSpecialKeyPress(int key, int x, int y){
    auto gameState = var::game->getGameState();

    switch (key) {
    case GLUT_KEY_SHIFT_L:
    case GLUT_KEY_SHIFT_R:
        gameState->keyShift = true;
        break;
    }
}

void onSpecialKeyRelease(int key, int x, int y){
    auto gameState = var::game->getGameState();

    switch (key) {
    case GLUT_KEY_SHIFT_L:
    case GLUT_KEY_SHIFT_R:
        gameState->keyShift = false;
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
    auto camera = var::game->getCamera();
    auto scene = var::game->getScene();
    auto gameState = var::game->getGameState();

    var::game->getDraw()->drawWindow(scene, camera, gameState);
    var::ui->renderUI();

    if (var::ui->getParamsChanged() && scene)
        scene->updatePlanet(var::ui->getPlanetParams());

    CHECK_GL_ERROR();
    glutSwapBuffers();

    FPS();
}

void uiKeyboardCallback(unsigned char key, int x, int y){
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);
}

void uiMouseCallback(int button, int state, int x, int y){
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}

void uiMotionCallback(int x, int y){
    ImGui_ImplGLUT_MotionFunc(x, y);
}

void uiReshapeCallback(int w, int h){
    ImGui_ImplGLUT_ReshapeFunc(w, h);
}
