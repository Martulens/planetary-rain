#include <iostream>
#include <list>
#include <set>
#include <utility>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL

#include "variables.h"

#include "framework.h"
#include "config.h"
#include "callbacks.h"
#include "game.h"
#include "camera.h"
#include "ui.h"

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

    // TODO -> does not work -\_(T.T)_/-
    // \todo Review: Use lambda's instead of callbacks. The lambda needs to have the same function signature as the callback requires
    // like this:
    // [](unsigned char keyPressed, int mouseX, int mouseY){}
    //
    // if you want to pass additional information it should passed via capture brackets.
    // lets say you implemented the game class as I have suggested and you have Game game; instance
    //
    // [&game](unsigned char keyPressed, int mouseX, int mouseY){...}
    // warning: if you pass [game], the instance is copied. I highly recommend getting familiar with lambdas
    // the lambda body is just as any function you would implement

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
