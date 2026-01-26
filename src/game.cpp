#include "game.h"
#include "camera.h"
#include "scene.h"

#include <GL/freeglut_std.h>
#include <GL/gl.h>

#include "glm/vec3.hpp"
#include "gamestate.h"

Game::Game(){
    mDraw = std::make_shared<Draw>();
}

void Game::restartGame()
{
    glClearColor(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

    cleanUpObjects();

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    if (h == 0)
        h = 1;

    // Initialize camera - positioned to see all three spheres
    glm::vec3 spherePos = SPHERE_POS;
    glm::vec3 camPosition = CAM_POS;
    glm::vec3 direction = spherePos;
    float angle = YAW;

    mCamera = std::make_shared<Camera>(w, h, camPosition, direction, angle);
    mGameState = std::make_shared<GameState>(w, h);

    mScene = std::make_shared<Scene>();
    mScene->init();
}

void Game::cleanUpObjects() {
    if (mScene)
        mScene = nullptr;
    if (mCamera)
        mCamera = nullptr;
    if (mGameState)
        mGameState = nullptr;
}