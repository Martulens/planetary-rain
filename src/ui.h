#pragma once

#ifndef UI_H
#define UI_H

#include <vector>

#include "noise.h"
#include "glm/vec3.hpp"
#include "noise.h"
#include "glm/vec2.hpp"

struct WaveSettings{
    float amplitude = 0.5f;
    float frequency = 2.0f;
    float speed = 3.5f;
    float steepness = 0.5f;
    glm::vec3 origin = glm::vec3(0.0f);
};

struct PlanetChangeFlags {
    bool meshChanged = false;
    bool materialChanged = false;
    bool noiseChanged = false;
    bool positionChanged = false;
    bool radiusChanged = false;
    bool animationChanged = false;
    bool wavesChanged = false;
    bool cloudsChanged = false;

    bool any() const {
        return meshChanged || materialChanged || noiseChanged
            || positionChanged || radiusChanged || animationChanged
            || wavesChanged || cloudsChanged;
    }
    void reset() {
        meshChanged = false;
        materialChanged = false;
        noiseChanged = false;
        positionChanged = false;
        radiusChanged = false;
        animationChanged = false;
        wavesChanged = false;
        cloudsChanged = false;
    }
};

struct PlanetParams {
    float x = 100.0f;
    float y = 0.0f;
    float z = 20.0f;

    float radius = 10.0f;
    int detail = 200;
    // Terrain colors (low / mid / high elevation)
    glm::vec3 color     = glm::vec3(0.20f, 0.40f, 1.00f); // mid
    glm::vec3 colorLow  = glm::vec3(0.12f, 0.25f, 0.55f);
    glm::vec3 colorHigh = glm::vec3(0.90f, 0.92f, 0.98f);

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

    // Waves
    bool wavesEnabled = false;
    float waveFadeE0 = 0.01f;
    float waveFadeE1 = 0.1f;
    float oceanLevel = 0.97f;

    int waveCount = 1;
    std::vector<WaveSettings> waves;

    // Rotation
    float rotationSpeed = 10.0f;
    bool autoRotate = false;
    float rotationAngle = 10.0f;

    // Clouds (volumetric shell above the planet)
    bool cloudsEnabled = false;
    float cloudAltitude = 1.0f;   // distance above the surface where the shell begins
    float cloudThickness = 0.6f;  // how tall the shell is (keep tight for speed)
    float cloudCoverage = 0.5f;   // 0 = clear, 1 = fully overcast
    float cloudDensity = 3.0f;    // extinction coefficient; higher = more opaque
    float cloudSpeed = 0.02f;     // drift speed of the noise field
    float cloudNoiseScale = 0.12f; // how "big" the cloud puffs are
    float cloudAlpha = 1.0f;      // final opacity multiplier
    float cloudBandStrength = 0.0f; // 0 = puffy, 1 = fully latitudinal bands
    float cloudBandFreq = 0.6f;   // number of bands along the y axis
    glm::vec3 cloudColor = glm::vec3(1.0f);

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
    void applyPreset(int type);

    // === GETTERS
    PlanetParams& getPlanetParams() { return mPlanetParams; };
    const PlanetChangeFlags& getChangeFlags() const { return mPlanetParams.changes; };
    bool getParamsChanged() const { return mPlanetParams.changes.any(); };
};
#endif