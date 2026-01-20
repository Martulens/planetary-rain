#include "game.h"
#include "camera.h"

#include <GL/freeglut_std.h>
#include <GL/gl.h>

#include "glm/vec3.hpp"
#include "context.h"
#include "gamestate.h"


void restartGame() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cleanUpObjects();
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    if (h == 0) h = 1;

    glm::vec3 camPosition = glm::vec3(0.0f, 2.0f, 10.0f);  // Start above ground, back from origin
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);    // Look toward -Z
    float angle = 0.0f;

    Camera* camera = new Camera(w, h, camPosition, direction, angle);
    GameState* gs = new GameState(w, h);
    con::init(camera, gs);
}

void cleanUpObjects() {
    if(con::camera)
        delete con::camera;
    if(con::gameState)
        delete con::gameState;
}