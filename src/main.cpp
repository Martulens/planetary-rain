#include <iostream>
#include <list>
#include <set>
#include <utility>
#include <algorithm>

#include "framework.h"
#include "config.h"
#include "callbacks.h"
#include "game.h"
#include "camera.h"
#include "context.h"

int lastMouseX = 0;
int lastMouseY = 0;

// === APP SETUP
void initApp() {
    srand((unsigned int)time(NULL));

    //background
    glClearColor(0.5f, 0.4f, 0.8f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    restartGame();
}

void finalizeApp() {
    cleanUpObjects();
}

int main(int argc, char** argv){
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
