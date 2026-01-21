#ifndef UI_H
#define UI_H

#include "glm/vec3.hpp"
#include "sphere.h"

struct PlanetParams {
    float x = 4.0f;
    float y = 1.5f;
    float z = 0.0f;

    float radius = 1.5f;
    int detail = 3;              // 1-4
    Terrain terrain;

    // Colour
    glm::vec3 color = glm::vec3(0.2f, 0.4f, 1.0f);

    // Material
    float pd = 0.6f;             // diffuse
    float ps = 0.5f;             // specular  
    float ns = 32.0f;            // shininess
    float reflectivity = 0.3f;

    float ior = 1.0f;
    float transparency = 1.0f;

    // Rotation
    float rotationSpeed = 30.0f;
    bool autoRotate = true;
    bool changed = false;
};

void initUI();
void renderUI(PlanetParams& params, bool& paramsChanged);
void shutdownUI();

// GLUT callback wrappers for ImGui
void uiKeyboardCallback(unsigned char key, int x, int y);
void uiMouseCallback(int button, int state, int x, int y);
void uiMotionCallback(int x, int y);
void uiReshapeCallback(int w, int h);

#endif