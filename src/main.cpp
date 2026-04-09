#include <iostream>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL

#include "variables.h"

#include "framework.h"
#include "config.h"
#include "callbacks.h"
#include "game.h"
#include "ui.h"

// === APP SETUP
void initApp() {
    srand((unsigned int)time(nullptr));
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

    Game game = Game();
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
        framework::dieWithError("Init failed, required OpenGL not supported?");

    var::getUI()->initUI();
    initApp();

    glutCloseFunc(finalizeApp);

    glutMainLoop();
    return 0;
}
