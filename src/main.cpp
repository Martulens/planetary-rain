#include <iostream>
#include <list>
#include <set>
#include <utility>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL

#include "framework.h"
#include "config.h"
#include "callbacks.h"
#include "game.h"
#include "camera.h"
#include "ui.h"
#include "variables.h"

int lastMouseX = 0;
int lastMouseY = 0;

// === APP SETUP
void initApp() {
    srand((unsigned int)time(NULL));
    glClearColor(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b, SKY_COLOR.w);
    glEnable(GL_DEPTH_TEST);

    var::getUI()->renderUI();
    var::getGame()->restartGame();
}

// === APP END
void finalizeApp() {
}

int main(int argc, char** argv){
    var::init();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Planetary Rain");

    glutSetCursor(GLUT_CURSOR_NONE);
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);

    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);

    glutSpecialFunc(onSpecialKeyPress);
    glutSpecialUpFunc(onSpecialKeyRelease);

    glutMouseFunc(onMouseClick);
    glutMotionFunc(onMouseMove);
    glutPassiveMotionFunc(onMouseMove);
    glutMouseWheelFunc(onMouseScroll);

    timer(0);

    if (!framework::initialize(framework::OGL_VER_MAJOR, framework::OGL_VER_MINOR))
        framework::dieWithError("pgr init failed, required OpenGL not supported?");

    initApp();

    glutCloseFunc(finalizeApp);

    glutMainLoop();
    return 0;
}
