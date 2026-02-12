#pragma once

#ifndef UI_H
#define UI_H

#include <vector>

#include "noise.h"
#include "glm/vec3.hpp"
#include "noise.h"

struct PlanetChangeFlags {
    bool meshChanged = false;
    bool materialChanged = false;
    bool noiseChanged = false;
    bool positionChanged = false;
    bool radiusChanged = false;
    bool animationChanged = false;

    bool any() const {
        return meshChanged || materialChanged || noiseChanged
            || positionChanged || radiusChanged || animationChanged;
    }
    void reset() {
        meshChanged = false;
        materialChanged = false;
        noiseChanged = false;
        positionChanged = false;
        radiusChanged = false;
        animationChanged = false;
    }
};

struct PlanetParams {
    float x = 100.0f;
    float y = 0.0f;
    float z = 20.0f;

    float radius = 10.0f;
    int detail = 30;
    // Color
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
    float power = 0.5;
    std::vector<NoiseSettings> noise;

    // Rotation
    float rotationSpeed = 30.0f;
    bool autoRotate = true;
    float rotationAngle = 0.0f;

    // Change tracking
    PlanetChangeFlags changes;

    PlanetParams(){
        noise.emplace_back();
    }
};

class UI{
private:
    PlanetParams mPlanetParams;
public:
    UI() = default;
    ~UI();

    void initUI();
    void renderUI();
    void randomize();

    // === GETTERS
    PlanetParams& getPlanetParams() { return mPlanetParams; };
    const PlanetChangeFlags& getChangeFlags() const { return mPlanetParams.changes; };
    bool getParamsChanged() const { return mPlanetParams.changes.any(); };
};
#endif