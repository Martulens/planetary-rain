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
#include "ui.h"

int lastMouseX = 0;
int lastMouseY = 0;

// === APP SETUP
void initApp() {
    srand((unsigned int)time(NULL));
    glClearColor(0.5f, 0.4f, 0.8f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    initUI();  // Add this
    restartGame();
}

void finalizeApp() {
    shutdownUI();
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

    glutKeyboardFunc([](unsigned char keyPressed, int mouseX, int mouseY){});
    glutKeyboardUpFunc(keyboardUpCallback);

    glutSpecialFunc(onSpecialKeyPress);
    glutSpecialUpFunc(onSpecialKeyRelease);

    glutMouseFunc(onMouseClick);
    glutMotionFunc(onMouseMove);
    glutPassiveMotionFunc(onMouseMove);
    glutMouseWheelFunc(onMouseScroll);

    timer(0);

    // \todo Review: this is a very strange way how to exit an app. The `dieWithError` has single usage and it does nothing extra
    // related to the framework. Why not just execute the return without the exit here.
    // the existence of dieWithError seems like unnecessary code bloat
    if (!framework::initialize(framework::OGL_VER_MAJOR, framework::OGL_VER_MINOR))
        framework::dieWithError("pgr init failed, required OpenGL not supported?");

    initApp();

    glutCloseFunc(finalizeApp);

    glutMainLoop();
    return 0;
}
