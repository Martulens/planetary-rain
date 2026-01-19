#include "utilities.h"

#include <iostream>
#include <GL/freeglut_std.h>

void FPS() {
    static float lastTime = 0.0f;
    static int frameCount = 0;

    float currentTime = 0.001f * glutGet(GLUT_ELAPSED_TIME);
    frameCount++;

    if (currentTime - lastTime >= 1.0f) {
        std::cout << "FPS: " << frameCount << std::endl;
        frameCount = 0;
        lastTime += 1.0f;
    }
}
