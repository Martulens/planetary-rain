#include "game.h"
#include "camera.h"
#include "scene.h"

#include <GL/freeglut_std.h>
#include <GL/gl.h>

#include "glm/vec3.hpp"
#include "context.h"
#include "gamestate.h"


void restartGame() {
    glClearColor(skyColorConst.r, skyColorConst.g, skyColorConst.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cleanUpObjects();

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    if (h == 0) h = 1;

    // Initialize camera - positioned to see all three spheres
    // \todo REVIEW practice to always write const, unless the value is modified. It helps readability but also is a best practice
    glm::vec3 spherePos = glm::vec3(4.0f, 1.5f, 0.0f);
    glm::vec3 camPosition = glm::vec3(0.0f, 4.0f, 12.0f);
    glm::vec3 direction = camPosition - spherePos;
    float angle = 0.0f;

    // \todo REVIEW if a game class is used, camera and game state, eventually scene could be member variables.
    //  This would help avoiding unintentional leaking if we were to change camera/scenes/state during the runtime
    Camera* camera = new Camera(w, h, camPosition, direction, angle);
    GameState* gs = new GameState(w, h);
    con::init(camera, gs);
}

void cleanUpObjects() {
    if (con::scene) {
        delete con::scene;
        con::scene = nullptr;
    }
    if (con::camera) {
        delete con::camera;
        con::camera = nullptr;
    }
    if (con::gameState) {
        delete con::gameState;
        con::gameState = nullptr;
    }
}