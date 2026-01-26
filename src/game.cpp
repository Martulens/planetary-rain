#include "game.h"
#include "camera.h"
#include "scene.h"

#include <GL/freeglut_std.h>
#include <GL/gl.h>

#include "glm/vec3.hpp"
#include "context.h"
#include "gamestate.h"

Game::Game(){
    draw = Draw();
}

void Game::restartGame() {
    glClearColor(skyColorConst.r, skyColorConst.g, skyColorConst.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

    cleanUpObjects();

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    if (h == 0) h = 1;

    // Initialize camera - positioned to see all three spheres
    glm::vec3 spherePos = glm::vec3(1.0f, 0.0f, -0.5f);
    glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction = spherePos;
    float angle = 270.0f;

    camera = std::make_shared<Camera>(w, h, camPosition, direction, angle);
    gameState = std::make_shared<GameState>(w, h);

    scene = std::make_shared<Scene>();
    scene->init();
}

void Game::cleanUpObjects() {
    if (scene) {
        delete scene;
        scene = nullptr;
    }
    if (camera) {
        delete camera;
        camera = nullptr;
    }
    if (gameState) {
        delete gameState;
        gameState = nullptr;
    }
}