#ifndef UI_H
#define UI_H

#include <vector>

#include "noise.h"
#include "glm/vec3.hpp"
#include "noise.h"

class PlanetParams {
public:
    float x = 100.0f;
    float y = 0.0f;
    float z = 20.0f;

    float radius = 10.0f;
    int detail = 30;
    // Colour
    glm::vec3 color = glm::vec3(0.2f, 0.4f, 1.0f);

    // Material
    float pd = 1.0f;             // diffuse
    float ps = 0.0f;             // specular
    float ns = 0.0f;            // shininess
    float reflectivity = 0.0f;

    float ior = 0.5f;
    float transparency = 1.0f;

    // Terrain
    bool showTerrain = false;
    int noiseCount = 1;
    std::vector<NoiseSettings> noise;

    // Rotation
    float rotationSpeed = 30.0f;
    bool autoRotate = true;
    bool changed = false;

    PlanetParams(){
        noise.emplace_back();
    }
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