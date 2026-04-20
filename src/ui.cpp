#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"
#include <GL/freeglut.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <random>

#include "glm/gtx/string_cast.hpp"

void UI::initUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.DisplaySize = ImVec2((float)glutGet(GLUT_WINDOW_WIDTH),
                            (float)glutGet(GLUT_WINDOW_HEIGHT));

    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 330");
}

void UI::randomize() {
    enum PlanetType { TERRESTRIAL, EARTHLIKE, DESERT, OCEAN };
    int type = rand() % 4;

    auto rf = [](float lo, float hi) {
        return lo + (rand() / (float)RAND_MAX) * (hi - lo);
    };

    auto makeNoise = [](bool  shown,
                        int   type,
                        int   octaves,
                        float frequency,
                        float amplitude,
                        float persistence,
                        float roughness,
                        float offset) {
        NoiseSettings n;
        n.shown       = shown;
        n.type        = type;
        n.octaves     = octaves;
        n.frequency   = frequency;
        n.amplitude   = amplitude + 1;
        n.persistence = persistence;
        n.roughness   = roughness;
        n.offset      = offset;
        return n;
    };

    mPlanetParams.reflectivity = 0.0f;
    mPlanetParams.ior          = 1.0f;
    mPlanetParams.transparency = 0.0f;

    std::cout << "[RANDOMIZE]: PlanetType " << type << std::endl;
    switch (type) {
    case TERRESTRIAL: {
        float grey          = rf(0.35f, 0.55f);
        mPlanetParams.color = glm::vec3(grey, grey * rf(0.9f, 1.0f), grey * rf(0.85f, 0.95f));
        mPlanetParams.pd    = rf(0.8f, 1.0f);
        mPlanetParams.ps    = rf(0.05f, 0.15f);
        mPlanetParams.ns    = rf(4.0f, 16.0f);

        mPlanetParams.noiseCount = 2 + rand() % 2;
        mPlanetParams.noise.clear();
        mPlanetParams.noise.push_back(
            makeNoise(true,
                      0,
                      1 + rand() % 4,
                      rf(0.6f, 1.0f),
                      rf(0.08f, 0.15f),
                      rf(0.45f, 0.55f),
                      rf(1.9f, 2.1f),
                      rf(0.0f, 5.0f)));
        mPlanetParams.noise.push_back(
            makeNoise(true,
                      rand() % 2,
                      4 + rand() % 3,
                      rf(1.5f, 2.5f),
                      rf(0.04f, 0.1f),
                      rf(0.4f, 0.5f),
                      rf(2.0f, 2.3f),
                      rf(0.0f, 5.0f)));
        if (mPlanetParams.noiseCount == 3)
            mPlanetParams.noise.push_back(
                makeNoise(true,
                          0,
                          3 + rand() % 3,
                          rf(3.0f, 4.0f),
                          rf(0.01f, 0.04f),
                          rf(0.35f, 0.45f),
                          rf(2.2f, 2.5f),
                          rf(0.0f, 5.0f)));
        break;
    }
    case EARTHLIKE: {
        mPlanetParams.color        = glm::vec3(rf(0.15f, 0.35f), rf(0.35f, 0.6f), rf(0.2f, 0.4f));
        mPlanetParams.pd           = rf(0.7f, 0.9f);
        mPlanetParams.ps           = rf(0.1f, 0.3f);
        mPlanetParams.ns           = rf(12.0f, 32.0f);
        mPlanetParams.reflectivity = rf(0.05f, 0.15f);

        mPlanetParams.noiseCount = 2 + rand() % 2;
        mPlanetParams.noise.clear();
        mPlanetParams.noise.push_back(
            makeNoise(true,
                      0,
                      3 + rand() % 2,
                      rf(0.4f, 0.8f),
                      rf(0.1f, 0.2f),
                      rf(0.45f, 0.55f),
                      rf(1.9f, 2.1f),
                      rf(0.0f, 5.0f)));
        mPlanetParams.noise.push_back(
            makeNoise(true,
                      rand() % 2,
                      4 + rand() % 3,
                      rf(1.5f, 2.5f),
                      rf(0.05f, 0.1f),
                      rf(0.4f, 0.5f),
                      rf(2.0f, 2.3f),
                      rf(0.0f, 5.0f)));
        if (mPlanetParams.noiseCount == 3)
            mPlanetParams.noise.push_back(
                makeNoise(true,
                          0,
                          5 + rand() % 2,
                          rf(3.0f, 4.0f),
                          rf(0.01f, 0.03f),
                          rf(0.35f, 0.45f),
                          rf(2.1f, 2.4f),
                          rf(0.0f, 5.0f)));
        break;
    }
    case DESERT: {
        mPlanetParams.color = glm::vec3(rf(0.7f, 0.9f), rf(0.5f, 0.7f), rf(0.3f, 0.45f));
        mPlanetParams.pd    = rf(0.85f, 1.0f);
        mPlanetParams.ps    = rf(0.02f, 0.1f);
        mPlanetParams.ns    = rf(4.0f, 12.0f);

        mPlanetParams.noiseCount = 2 + rand() % 2;
        mPlanetParams.noise.clear();
        mPlanetParams.noise.push_back(
            makeNoise(true,
                      0,
                      3 + rand() % 2,
                      rf(0.5f, 0.9f),
                      rf(0.06f, 0.12f),
                      rf(0.5f, 0.6f),
                      rf(1.9f, 2.1f),
                      rf(0.0f, 5.0f)));
        mPlanetParams.noise.push_back(
            makeNoise(true,
                      2,
                      4 + rand() % 2,
                      rf(1.2f, 2.0f),
                      rf(0.05f, 0.1f),
                      rf(0.4f, 0.55f),
                      rf(2.0f, 2.3f),
                      rf(0.2f, 0.8f)));
        if (mPlanetParams.noiseCount == 3)
            mPlanetParams.noise.push_back(
                makeNoise(true,
                          0,
                          5 + rand() % 3,
                          rf(2.5f, 3.5f),
                          rf(0.01f, 0.03f),
                          rf(0.35f, 0.45f),
                          rf(2.1f, 2.4f),
                          rf(0.0f, 5.0f)));
        break;
    }
    case OCEAN: {
        mPlanetParams.color        = glm::vec3(rf(0.05f, 0.2f), rf(0.2f, 0.45f), rf(0.5f, 0.8f));
        mPlanetParams.pd           = rf(0.5f, 0.7f);
        mPlanetParams.ps           = rf(0.4f, 0.7f);
        mPlanetParams.ns           = rf(32.0f, 96.0f);
        mPlanetParams.reflectivity = rf(0.1f, 0.3f);
        mPlanetParams.transparency = rf(0.05f, 0.15f);
        mPlanetParams.ior          = rf(1.3f, 1.4f);

        mPlanetParams.noiseCount = 0;
        mPlanetParams.noise.clear();
        break;
    }
    }

    // Derive low/high terrain shades from the mid color so all three slots move together
    mPlanetParams.colorLow  = mPlanetParams.color * 0.5f;
    mPlanetParams.colorHigh = glm::mix(mPlanetParams.color, glm::vec3(1.0f), 0.55f);

    // Randomize changes material + noise, not mesh detail
    mPlanetParams.changes.materialChanged = true;
    mPlanetParams.changes.noiseChanged    = true;
    mPlanetParams.changes.radiusChanged   = true;
}

void randomizeWaves(PlanetParams& params, int count);
void randomizeClouds(PlanetParams& params);

enum Preset {
    PRESET_EARTHLIKE = 0,
    PRESET_TERRESTRIAL,
    PRESET_ICY,
    PRESET_BARREN,
    PRESET_VOLCANIC,
    PRESET_GAS_GIANT,
    PRESET_OCEAN,
};

static float presetRf(float lo, float hi) {
    return lo + (rand() / (float)RAND_MAX) * (hi - lo);
}

static int presetRi(int lo, int hi) {
    return lo + rand() % (hi - lo + 1);
}

static glm::vec3 presetCol(float rLo,
                           float rHi,
                           float gLo,
                           float gHi,
                           float bLo,
                           float bHi) {
    return glm::vec3(presetRf(rLo, rHi), presetRf(gLo, gHi), presetRf(bLo, bHi));
}

static NoiseSettings mkNoise(int   type,
                             int   octaves,
                             float freq,
                             float amp,
                             float persistence,
                             float roughness,
                             float offset) {
    NoiseSettings n;
    n.shown       = true;
    n.type        = type;
    n.octaves     = octaves;
    n.frequency   = freq;
    n.amplitude   = amp;
    n.persistence = persistence;
    n.roughness   = roughness;
    n.offset      = offset;
    return n;
}

void UI::applyPreset(int type) {
    PlanetParams& p = mPlanetParams;

    p.reflectivity  = 0.0f;
    p.ior           = 1.0f;
    p.transparency  = 0.0f;
    p.showTerrain   = true;
    p.wavesEnabled  = false;
    p.oceanLevel    = 0.0f;  // waterless baseline; Earthlike/Ocean override
    p.cloudsEnabled = false; // individual presets enable clouds when appropriate
    p.noise.clear();

    switch (type) {
    case PRESET_EARTHLIKE: {
        int mood = rand() % 4;
        if (mood == 0) {
            // classic earth (old hardcoded palette)
            p.colorLow  = presetCol(0.85f, 0.95f, 0.75f, 0.85f, 0.55f, 0.65f); // sand
            p.color     = presetCol(0.15f, 0.25f, 0.55f, 0.65f, 0.15f, 0.25f); // green
            p.colorHigh = presetCol(0.92f, 1.00f, 0.92f, 1.00f, 0.92f, 1.00f); // snow
        } else if (mood == 1) {
            // temperate green
            p.colorLow  = presetCol(0.65f, 0.82f, 0.58f, 0.72f, 0.38f, 0.52f); // sand
            p.color     = presetCol(0.15f, 0.28f, 0.42f, 0.58f, 0.25f, 0.40f); // forest
            p.colorHigh = presetCol(0.85f, 0.98f, 0.88f, 0.98f, 0.90f, 1.00f); // snow
        } else if (mood == 2) {
            // savanna / tropical
            p.colorLow  = presetCol(0.45f, 0.60f, 0.35f, 0.45f, 0.20f, 0.30f); // brown
            p.color     = presetCol(0.55f, 0.70f, 0.55f, 0.65f, 0.20f, 0.35f); // yellow-green
            p.colorHigh = presetCol(0.82f, 0.95f, 0.78f, 0.90f, 0.62f, 0.75f); // pale tan
        } else {
            // alien violet-green
            p.colorLow  = presetCol(0.18f, 0.30f, 0.10f, 0.20f, 0.30f, 0.45f); // dark violet
            p.color     = presetCol(0.35f, 0.50f, 0.30f, 0.45f, 0.50f, 0.65f); // violet-green
            p.colorHigh = presetCol(0.70f, 0.88f, 0.90f, 1.00f, 0.85f, 1.00f); // cyan-white
        }

        p.pd           = presetRf(0.80f, 0.90f);
        p.ps           = presetRf(0.15f, 0.28f);
        p.ns           = presetRf(20.0f, 34.0f);
        p.reflectivity = presetRf(0.06f, 0.14f);
        p.noise.push_back(mkNoise(0,
                                  presetRi(3, 5),
                                  presetRf(0.5f, 0.8f),
                                  presetRf(1.10f, 1.20f),
                                  presetRf(0.45f, 0.55f),
                                  presetRf(1.9f, 2.1f),
                                  presetRf(0.0f, 5.0f)));
        p.noise.push_back(mkNoise(rand() % 2,
                                  presetRi(4, 6),
                                  presetRf(1.5f, 2.3f),
                                  presetRf(1.03f, 1.10f),
                                  presetRf(0.40f, 0.50f),
                                  presetRf(2.1f, 2.3f),
                                  presetRf(0.0f, 5.0f)));
        p.noise.push_back(mkNoise(0,
                                  presetRi(5, 7),
                                  presetRf(2.8f, 3.8f),
                                  presetRf(1.01f, 1.04f),
                                  presetRf(0.35f, 0.45f),
                                  presetRf(2.2f, 2.4f),
                                  presetRf(0.0f, 5.0f)));
        p.noiseCount   = 3;
        p.wavesEnabled = true;
        p.oceanLevel   = presetRf(0.94f, 1.34f);
        randomizeWaves(p, presetRi(3, 5));

        p.cloudsEnabled     = true;
        p.cloudAltitude     = presetRf(8.0f, 20.0f);
        p.cloudThickness    = presetRf(0.4f, 0.8f);
        p.cloudCoverage     = presetRf(0.4f, 0.6f);
        p.cloudDensity      = presetRf(2.5f, 4.0f);
        p.cloudSpeed        = presetRf(0.01f, 0.04f);
        p.cloudNoiseScale   = presetRf(0.10f, 0.18f);
        p.cloudAlpha        = presetRf(0.75f, 1.0f);
        p.cloudBandStrength = 0.0f;
        p.cloudBandFreq     = 0.6f;
        p.cloudColor        = glm::vec3(1.0f);
        break;
    }

    case PRESET_TERRESTRIAL: {
        int mood = rand() % 3;
        if (mood == 0) {
            // rocky brown
            p.colorLow  = presetCol(0.18f, 0.30f, 0.14f, 0.22f, 0.10f, 0.18f);
            p.color     = presetCol(0.45f, 0.58f, 0.38f, 0.48f, 0.28f, 0.38f);
            p.colorHigh = presetCol(0.75f, 0.88f, 0.68f, 0.80f, 0.55f, 0.68f);
        } else if (mood == 1) {
            // desert tan
            p.colorLow  = presetCol(0.55f, 0.70f, 0.35f, 0.45f, 0.22f, 0.32f);
            p.color     = presetCol(0.75f, 0.88f, 0.62f, 0.75f, 0.42f, 0.55f);
            p.colorHigh = presetCol(0.92f, 1.00f, 0.86f, 0.96f, 0.70f, 0.85f);
        } else {
            // red canyon
            p.colorLow  = presetCol(0.30f, 0.45f, 0.12f, 0.22f, 0.08f, 0.16f);
            p.color     = presetCol(0.55f, 0.70f, 0.30f, 0.42f, 0.22f, 0.32f);
            p.colorHigh = presetCol(0.85f, 0.98f, 0.62f, 0.78f, 0.50f, 0.65f);
        }

        p.pd = presetRf(0.88f, 0.98f);
        p.ps = presetRf(0.04f, 0.12f);
        p.ns = presetRf(6.0f, 12.0f);
        p.noise.push_back(mkNoise(2,
                                  presetRi(4, 6),
                                  presetRf(0.7f, 1.0f),
                                  presetRf(1.14f, 1.22f),
                                  presetRf(0.48f, 0.56f),
                                  presetRf(1.9f, 2.1f),
                                  presetRf(0.2f, 0.8f)));
        p.noise.push_back(mkNoise(0,
                                  presetRi(4, 6),
                                  presetRf(1.7f, 2.3f),
                                  presetRf(1.05f, 1.12f),
                                  presetRf(0.42f, 0.50f),
                                  presetRf(2.1f, 2.3f),
                                  presetRf(0.0f, 5.0f)));
        p.noise.push_back(mkNoise(1,
                                  presetRi(5, 7),
                                  presetRf(3.0f, 4.0f),
                                  presetRf(1.01f, 1.05f),
                                  presetRf(0.38f, 0.44f),
                                  presetRf(2.2f, 2.4f),
                                  presetRf(0.0f, 5.0f)));
        p.noiseCount = 3;
        break;
    }

    case PRESET_ICY: {
        int mood = rand() % 3;
        if (mood == 0) {
            // pale blue
            p.colorLow  = presetCol(0.38f, 0.55f, 0.62f, 0.78f, 0.75f, 0.88f);
            p.color     = presetCol(0.78f, 0.90f, 0.86f, 0.95f, 0.94f, 1.00f);
            p.colorHigh = presetCol(0.95f, 1.00f, 0.96f, 1.00f, 0.98f, 1.00f);
        } else if (mood == 1) {
            // mint
            p.colorLow  = presetCol(0.45f, 0.62f, 0.68f, 0.82f, 0.65f, 0.78f);
            p.color     = presetCol(0.82f, 0.92f, 0.95f, 1.00f, 0.88f, 0.96f);
            p.colorHigh = presetCol(0.95f, 1.00f, 0.98f, 1.00f, 0.95f, 1.00f);
        } else {
            // frost violet
            p.colorLow  = presetCol(0.45f, 0.60f, 0.40f, 0.55f, 0.55f, 0.70f);
            p.color     = presetCol(0.83f, 0.92f, 0.80f, 0.90f, 0.92f, 1.00f);
            p.colorHigh = presetCol(0.95f, 1.00f, 0.95f, 1.00f, 0.98f, 1.00f);
        }

        p.pd           = presetRf(0.50f, 0.62f);
        p.ps           = presetRf(0.55f, 0.70f);
        p.ns           = presetRf(64.0f, 110.0f);
        p.reflectivity = presetRf(0.28f, 0.45f);
        p.ior          = presetRf(1.28f, 1.34f);
        p.noise.push_back(mkNoise(0,
                                  presetRi(2, 4),
                                  presetRf(0.4f, 0.7f),
                                  presetRf(1.02f, 1.06f),
                                  presetRf(0.45f, 0.55f),
                                  presetRf(1.9f, 2.1f),
                                  presetRf(0.0f, 5.0f)));
        p.noise.push_back(mkNoise(2,
                                  presetRi(3, 5),
                                  presetRf(1.4f, 2.0f),
                                  presetRf(1.01f, 1.03f),
                                  presetRf(0.40f, 0.50f),
                                  presetRf(2.1f, 2.3f),
                                  presetRf(0.2f, 0.8f)));
        p.noiseCount = 2;
        break;
    }

    case PRESET_BARREN: {
        int mood = rand() % 3;
        if (mood == 0) {
            // moon grey
            p.colorLow  = presetCol(0.14f, 0.22f, 0.14f, 0.22f, 0.14f, 0.22f);
            p.color     = presetCol(0.40f, 0.50f, 0.40f, 0.48f, 0.38f, 0.45f);
            p.colorHigh = presetCol(0.75f, 0.90f, 0.75f, 0.88f, 0.72f, 0.85f);
        } else if (mood == 1) {
            // mercury tan
            p.colorLow  = presetCol(0.22f, 0.32f, 0.16f, 0.26f, 0.12f, 0.20f);
            p.color     = presetCol(0.55f, 0.68f, 0.45f, 0.55f, 0.35f, 0.45f);
            p.colorHigh = presetCol(0.85f, 0.96f, 0.75f, 0.88f, 0.60f, 0.72f);
        } else {
            // dark basalt
            p.colorLow  = presetCol(0.05f, 0.10f, 0.05f, 0.10f, 0.06f, 0.12f);
            p.color     = presetCol(0.18f, 0.28f, 0.18f, 0.26f, 0.20f, 0.28f);
            p.colorHigh = presetCol(0.50f, 0.65f, 0.50f, 0.65f, 0.55f, 0.70f);
        }

        p.pd = presetRf(0.88f, 0.96f);
        p.ps = presetRf(0.02f, 0.08f);
        p.ns = presetRf(4.0f, 10.0f);
        p.noise.push_back(mkNoise(0,
                                  presetRi(3, 5),
                                  presetRf(0.6f, 0.9f),
                                  presetRf(1.06f, 1.14f),
                                  presetRf(0.45f, 0.55f),
                                  presetRf(1.9f, 2.1f),
                                  presetRf(0.0f, 5.0f)));
        p.noise.push_back(mkNoise(1,
                                  presetRi(4, 6),
                                  presetRf(2.0f, 2.6f),
                                  presetRf(1.03f, 1.08f),
                                  presetRf(0.40f, 0.50f),
                                  presetRf(2.1f, 2.3f),
                                  presetRf(0.0f, 5.0f)));
        p.noise.push_back(mkNoise(2,
                                  presetRi(5, 7),
                                  presetRf(3.5f, 4.5f),
                                  presetRf(1.01f, 1.04f),
                                  presetRf(0.36f, 0.44f),
                                  presetRf(2.3f, 2.5f),
                                  presetRf(0.2f, 0.8f)));
        p.noiseCount = 3;
        break;
    }

    case PRESET_VOLCANIC: {
        int mood = rand() % 3;
        if (mood == 0) {
            // classic lava
            p.colorLow  = presetCol(0.08f, 0.18f, 0.02f, 0.06f, 0.02f, 0.05f); // charred
            p.color     = presetCol(0.50f, 0.65f, 0.10f, 0.20f, 0.04f, 0.10f); // lava red
            p.colorHigh = presetCol(0.85f, 1.00f, 0.42f, 0.58f, 0.10f, 0.22f); // molten orange
        } else if (mood == 1) {
            // molten orange
            p.colorLow  = presetCol(0.18f, 0.28f, 0.08f, 0.14f, 0.04f, 0.10f);
            p.color     = presetCol(0.70f, 0.85f, 0.30f, 0.45f, 0.05f, 0.15f);
            p.colorHigh = presetCol(0.95f, 1.00f, 0.75f, 0.92f, 0.18f, 0.35f);
        } else {
            // dark charred
            p.colorLow  = presetCol(0.03f, 0.08f, 0.02f, 0.06f, 0.02f, 0.06f);
            p.color     = presetCol(0.20f, 0.32f, 0.08f, 0.15f, 0.08f, 0.16f);
            p.colorHigh = presetCol(0.52f, 0.68f, 0.20f, 0.32f, 0.15f, 0.26f);
        }

        p.pd = presetRf(0.95f, 1.00f);
        p.ps = presetRf(0.12f, 0.25f);
        p.ns = presetRf(8.0f, 14.0f);
        p.noise.push_back(mkNoise(2,
                                  presetRi(4, 6),
                                  presetRf(0.8f, 1.1f),
                                  presetRf(1.16f, 1.24f),
                                  presetRf(0.50f, 0.58f),
                                  presetRf(2.0f, 2.2f),
                                  presetRf(0.2f, 0.8f)));
        p.noise.push_back(mkNoise(1,
                                  presetRi(4, 6),
                                  presetRf(2.1f, 2.7f),
                                  presetRf(1.07f, 1.13f),
                                  presetRf(0.45f, 0.52f),
                                  presetRf(2.2f, 2.4f),
                                  presetRf(0.0f, 5.0f)));
        p.noise.push_back(mkNoise(0,
                                  presetRi(5, 7),
                                  presetRf(3.4f, 4.2f),
                                  presetRf(1.02f, 1.06f),
                                  presetRf(0.40f, 0.46f),
                                  presetRf(2.3f, 2.5f),
                                  presetRf(0.0f, 5.0f)));
        p.noiseCount = 3;
        break;
    }

    case PRESET_GAS_GIANT: {
        int mood = rand() % 4;
        if (mood == 0) {
            // jupiter
            p.colorLow  = presetCol(0.38f, 0.52f, 0.24f, 0.34f, 0.10f, 0.20f);
            p.color     = presetCol(0.72f, 0.85f, 0.50f, 0.62f, 0.28f, 0.40f);
            p.colorHigh = presetCol(0.92f, 1.00f, 0.82f, 0.92f, 0.65f, 0.80f);
        } else if (mood == 1) {
            // saturn
            p.colorLow  = presetCol(0.55f, 0.68f, 0.42f, 0.55f, 0.28f, 0.40f);
            p.color     = presetCol(0.82f, 0.92f, 0.75f, 0.85f, 0.55f, 0.68f);
            p.colorHigh = presetCol(0.95f, 1.00f, 0.92f, 1.00f, 0.85f, 0.95f);
        } else if (mood == 2) {
            // neptune
            p.colorLow  = presetCol(0.06f, 0.14f, 0.12f, 0.22f, 0.40f, 0.55f);
            p.color     = presetCol(0.20f, 0.32f, 0.35f, 0.50f, 0.70f, 0.85f);
            p.colorHigh = presetCol(0.55f, 0.72f, 0.75f, 0.90f, 0.92f, 1.00f);
        } else {
            // uranus
            p.colorLow  = presetCol(0.22f, 0.38f, 0.55f, 0.68f, 0.62f, 0.75f);
            p.color     = presetCol(0.55f, 0.70f, 0.82f, 0.92f, 0.85f, 0.95f);
            p.colorHigh = presetCol(0.90f, 1.00f, 0.95f, 1.00f, 0.95f, 1.00f);
        }

        p.pd          = presetRf(0.95f, 1.00f);
        p.ps          = presetRf(0.15f, 0.28f);
        p.ns          = presetRf(12.0f, 22.0f);
        p.showTerrain = false;
        p.noise.push_back(mkNoise(0,
                                  presetRi(2, 3),
                                  presetRf(0.25f, 0.4f),
                                  presetRf(1.01f, 1.04f),
                                  presetRf(0.45f, 0.55f),
                                  presetRf(1.8f, 2.0f),
                                  presetRf(0.0f, 5.0f)));
        p.noiseCount = 1;

        // Banded atmospheric cloud layer - tinted to match the gas giant mood
        p.cloudsEnabled     = true;
        p.cloudAltitude     = presetRf(8.0f, 20.0f);
        p.cloudThickness    = presetRf(1.2f, 2.2f);
        p.cloudCoverage     = presetRf(0.70f, 0.88f);
        p.cloudDensity      = presetRf(3.0f, 5.0f);
        p.cloudSpeed        = presetRf(0.03f, 0.08f);
        p.cloudNoiseScale   = presetRf(0.05f, 0.10f);
        p.cloudAlpha        = presetRf(0.7f, 0.9f);
        p.cloudBandStrength = presetRf(0.65f, 0.95f);
        p.cloudBandFreq     = presetRf(0.8f, 1.8f);
        p.cloudColor        = glm::mix(p.color, glm::vec3(1.0f), 0.6f);
        break;
    }

    case PRESET_OCEAN: {
        int mood = rand() % 3;
        if (mood == 0) {
            // deep blue
            p.colorLow  = presetCol(0.02f, 0.08f, 0.10f, 0.20f, 0.28f, 0.42f); // abyss
            p.color     = presetCol(0.05f, 0.15f, 0.22f, 0.34f, 0.55f, 0.70f); // ocean
            p.colorHigh = presetCol(0.85f, 1.00f, 0.90f, 1.00f, 0.95f, 1.00f); // foam
        } else if (mood == 1) {
            // tropical
            p.colorLow  = presetCol(0.03f, 0.10f, 0.25f, 0.38f, 0.35f, 0.50f);
            p.color     = presetCol(0.08f, 0.20f, 0.45f, 0.60f, 0.55f, 0.70f);
            p.colorHigh = presetCol(0.72f, 0.90f, 0.92f, 1.00f, 0.92f, 1.00f);
        } else {
            // alien violet
            p.colorLow  = presetCol(0.10f, 0.20f, 0.05f, 0.12f, 0.30f, 0.45f);
            p.color     = presetCol(0.25f, 0.40f, 0.15f, 0.28f, 0.55f, 0.72f);
            p.colorHigh = presetCol(0.82f, 0.95f, 0.78f, 0.90f, 0.92f, 1.00f);
        }

        p.pd           = presetRf(0.55f, 0.68f);
        p.ps           = presetRf(0.55f, 0.72f);
        p.ns           = presetRf(80.0f, 120.0f);
        p.reflectivity = presetRf(0.38f, 0.55f);
        p.transparency = presetRf(0.06f, 0.14f);
        p.ior          = presetRf(1.30f, 1.36f);

        p.noiseCount   = 0;
        p.wavesEnabled = true;
        p.oceanLevel   = presetRf(0.92f, 1.5f);
        randomizeWaves(p, presetRi(4, 6));
        break;
    }
    }

    p.changes.materialChanged = true;
    p.changes.noiseChanged    = true;
    p.changes.radiusChanged   = true;
    p.changes.wavesChanged    = true;
}

void randomizeClouds(PlanetParams& params) {
    // Pick a mood: 0 = wispy, 1 = overcast, 2 = broken/scattered, 3 = stormy giant-bands
    int mood = rand() % 4;

    auto rf = [](float lo, float hi) {
        return lo + (rand() / (float)RAND_MAX) * (hi - lo);
    };

    switch (mood) {
    case 0: // wispy high cirrus
        params.cloudAltitude     = rf(10.0f, 20.0f);
        params.cloudThickness    = rf(0.3f, 0.6f);
        params.cloudCoverage     = rf(0.25f, 0.45f);
        params.cloudDensity      = rf(1.5f, 3.0f);
        params.cloudNoiseScale   = rf(0.05f, 0.10f);
        params.cloudAlpha        = rf(0.5f, 0.8f);
        params.cloudBandStrength = 0.0f;
        params.cloudBandFreq     = 0.6f;
        params.cloudColor        = glm::vec3(rf(0.92f, 1.0f), rf(0.92f, 1.0f), rf(0.95f, 1.0f));
        break;
    case 1: // thick overcast
        params.cloudAltitude     = rf(3.0f, 10.0f);
        params.cloudThickness    = rf(0.6f, 1.2f);
        params.cloudCoverage     = rf(0.65f, 0.85f);
        params.cloudDensity      = rf(4.0f, 7.0f);
        params.cloudNoiseScale   = rf(0.08f, 0.14f);
        params.cloudAlpha        = rf(0.85f, 1.0f);
        params.cloudBandStrength = 0.0f;
        params.cloudBandFreq     = 0.6f;
        params.cloudColor        = glm::vec3(rf(0.85f, 0.95f), rf(0.88f, 0.96f), rf(0.92f, 1.0f));
        break;
    case 2: // broken/scattered
        params.cloudAltitude     = rf(6.0f, 14.0f);
        params.cloudThickness    = rf(0.4f, 0.9f);
        params.cloudCoverage     = rf(0.40f, 0.60f);
        params.cloudDensity      = rf(3.0f, 5.0f);
        params.cloudNoiseScale   = rf(0.10f, 0.18f);
        params.cloudAlpha        = rf(0.75f, 1.0f);
        params.cloudBandStrength = 0.0f;
        params.cloudBandFreq     = 0.6f;
        params.cloudColor        = glm::vec3(rf(0.90f, 1.0f), rf(0.90f, 1.0f), rf(0.90f, 1.0f));
        break;
    default: // gas-giant bands
        params.cloudAltitude     = rf(10.0f, 20.0f);
        params.cloudThickness    = rf(1.2f, 2.2f);
        params.cloudCoverage     = rf(0.70f, 0.88f);
        params.cloudDensity      = rf(3.0f, 5.0f);
        params.cloudNoiseScale   = rf(0.04f, 0.08f);
        params.cloudAlpha        = rf(0.7f, 0.9f);
        params.cloudBandStrength = rf(0.65f, 0.95f);
        params.cloudBandFreq     = rf(0.8f, 1.8f);
        params.cloudColor        = glm::vec3(rf(0.6f, 1.0f), rf(0.5f, 1.0f), rf(0.5f, 1.0f));
        break;
    }

    params.cloudSpeed = rf(0.005f, 0.08f);
    params.cloudsEnabled = true;
}

// Realistic randomization for spherical Gerstner waves
void randomizeWaves(PlanetParams& params, int count = 5) {
    std::mt19937 rng(std::random_device{}());

    params.wavesEnabled = true;
    params.waveCount    = count;
    params.waveFadeE0   = 0.01f;
    params.waveFadeE1   = 0.1f;
    params.waves.clear();

    //Distribution helpers
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> cosThetaDist(-1.0f, 1.0f);

    for (int i = 0; i < count; i++) {
        WaveSettings set;

        //Amplitude decreases with each wave layer
        float                                 baseAmp = 3.0f * std::pow(0.55f, static_cast<float>(i));
        std::uniform_real_distribution<float> ampJitter(0.8f, 1.2f);
        float                                 val = baseAmp * ampJitter(rng);
        set.amplitude                             = glm::clamp(val, 0.01f, 0.5f);

        // Frequency increases with each layer(shorter wavelengthsfor detail)
        float                                 baseFreq = 0.05f * std::pow(1.8f, static_cast<float>(i));
        std::uniform_real_distribution<float> freqJitter(0.85f, 7.5f);
        set.frequency = glm::clamp(baseFreq * freqJitter(rng), 0.5f, 3.5f);

        // Speed(phase velocity) - bigger
        float                                 baseSpeed = 0.15f * std::pow(0.7f, static_cast<float>(i));
        std::uniform_real_distribution<float> speedJitter(1.8f, 5.0f);
        set.speed = glm::clamp(baseSpeed * speedJitter(rng), 1.8f, 5.0f);

        //Steepness Q :
        float                                 baseSteep = 0.3f + 0.15f * i;
        std::uniform_real_distribution<float> steepJitter(0.9f, 1.1f);
        set.steepness = glm::clamp(baseSteep * steepJitter(rng), 0.1f, 0.75f);

        // Origin:
        // random point on unit sphere(the shader expects a direction)
        // Using uniform sphere sampling
        float cosTheta = cosThetaDist(rng);
        float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
        float phi      = angleDist(rng);
        set.origin     = glm::vec3(
            sinTheta * std::cos(phi),
            sinTheta * std::sin(phi),
            cosTheta
            );

        params.waves.emplace_back(set);
    }
}

void UI::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    mPlanetParams.changes.reset();

    ImGui::Begin("Planet Editor");

    if (ImGui::Button("Randomize"))
        randomize();

    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Press TAB to toggle mouse");
    ImGui::Separator();

    // --- Geometry ---
    if (ImGui::CollapsingHeader("Geometry", ImGuiTreeNodeFlags_DefaultOpen)) {
        mPlanetParams.changes.radiusChanged |= ImGui::SliderFloat(
            "Radius",
            &mPlanetParams.radius,
            0.5f,
            50.0f);
        mPlanetParams.changes.meshChanged |= ImGui::SliderInt("Detail", &mPlanetParams.detail, 1, 500);
        mPlanetParams.changes.positionChanged |= ImGui::SliderFloat("x", &mPlanetParams.x, -100.0f, 150.0f);
        mPlanetParams.changes.positionChanged |= ImGui::SliderFloat("y", &mPlanetParams.y, -100.0f, 150.0f);
        mPlanetParams.changes.positionChanged |= ImGui::SliderFloat("z", &mPlanetParams.z, -100.0f, 150.0f);
    }

    // Animation
    if (ImGui::CollapsingHeader("Animation")) {
        mPlanetParams.changes.animationChanged |= ImGui::Checkbox("Auto Rotate", &mPlanetParams.autoRotate);
        mPlanetParams.changes.animationChanged |= ImGui::SliderFloat("Speed (deg/s)",
                                                                     &mPlanetParams.rotationSpeed,
                                                                     0.0f,
                                                                     180.0f);

        if (!mPlanetParams.autoRotate) {
            mPlanetParams.changes.animationChanged |= ImGui::SliderFloat(
                "Angle",
                &mPlanetParams.rotationAngle,
                0.0f,
                360.0f);
        }
    }

    // --- Clouds ---
    if (ImGui::CollapsingHeader("Clouds")) {
        mPlanetParams.changes.cloudsChanged |= ImGui::Checkbox("Enable Clouds", &mPlanetParams.cloudsEnabled);
        if (mPlanetParams.cloudsEnabled) {
            if (ImGui::Button("Randomize Clouds")) {
                randomizeClouds(mPlanetParams);
                mPlanetParams.changes.cloudsChanged = true;
            }
            mPlanetParams.changes.cloudsChanged |= ImGui::SliderFloat(
                "Altitude",
                &mPlanetParams.cloudAltitude,
                0.0f,
                30.0f);
            mPlanetParams.changes.cloudsChanged |= ImGui::SliderFloat(
                "Thickness",
                &mPlanetParams.cloudThickness,
                0.1f,
                4.0f);
            mPlanetParams.changes.cloudsChanged |= ImGui::SliderFloat(
                "Coverage",
                &mPlanetParams.cloudCoverage,
                0.0f,
                1.0f);
            mPlanetParams.changes.cloudsChanged |= ImGui::SliderFloat(
                "Density",
                &mPlanetParams.cloudDensity,
                0.1f,
                15.0f);
            mPlanetParams.changes.cloudsChanged |= ImGui::SliderFloat(
                "Drift Speed",
                &mPlanetParams.cloudSpeed,
                0.0f,
                1.0f);
            mPlanetParams.changes.cloudsChanged |= ImGui::SliderFloat(
                "Noise Scale",
                &mPlanetParams.cloudNoiseScale,
                0.01f,
                1.0f);
            mPlanetParams.changes.cloudsChanged |= ImGui::SliderFloat(
                "Alpha",
                &mPlanetParams.cloudAlpha,
                0.0f,
                1.0f);
            mPlanetParams.changes.cloudsChanged |= ImGui::SliderFloat(
                "Band Strength",
                &mPlanetParams.cloudBandStrength,
                0.0f,
                1.0f);
            mPlanetParams.changes.cloudsChanged |= ImGui::SliderFloat(
                "Band Frequency",
                &mPlanetParams.cloudBandFreq,
                0.1f,
                3.0f);
            mPlanetParams.changes.cloudsChanged |= ImGui::ColorEdit3(
                "Cloud Color",
                &mPlanetParams.cloudColor.x);
        }
    }

    // --- Waves ---
    if (ImGui::CollapsingHeader("Waves")) {
        mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat("Ocean Level",
                                                                 &mPlanetParams.oceanLevel,
                                                                 0.0f,
                                                                 2.0f);
        mPlanetParams.changes.wavesChanged |= ImGui::Checkbox("Enable Waves", &mPlanetParams.wavesEnabled);

        if (mPlanetParams.wavesEnabled) {
            if (ImGui::Button("Randomize Waves")) {
                randomizeWaves(mPlanetParams);
                mPlanetParams.changes.wavesChanged = true;
            }

            mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat("Wave FadeE0",
                                                                     &mPlanetParams.waveFadeE0,
                                                                     0.001f,
                                                                     0.05f);
            mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat("Wave FadeE1",
                                                                     &mPlanetParams.waveFadeE1,
                                                                     0.01f,
                                                                     0.5f);

            mPlanetParams.changes.wavesChanged |= ImGui::SliderInt(
                "Number of Waves",
                &mPlanetParams.waveCount,
                0,
                10);

            while ((int)mPlanetParams.waves.size() < mPlanetParams.waveCount)
                mPlanetParams.waves.emplace_back();
            while ((int)mPlanetParams.waves.size() > mPlanetParams.waveCount)
                mPlanetParams.waves.pop_back();

            for (int i = 0; i < mPlanetParams.waveCount; i++) {
                ImGui::PushID(i);

                std::string name = "Wave " + std::to_string(i);
                if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Frequency",
                        &mPlanetParams.waves[i].frequency,
                        0.01f,
                        3.5f);
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Speed",
                        &mPlanetParams.waves[i].speed,
                        0.0f,
                        5.0f);
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Steepness",
                        &mPlanetParams.waves[i].steepness,
                        0.0f,
                        0.85f);

                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Origin X",
                        &mPlanetParams.waves[i].origin.x,
                        -1.0f,
                        1.0f);
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Origin Y",
                        &mPlanetParams.waves[i].origin.y,
                        -1.0f,
                        1.0f);
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Origin Z",
                        &mPlanetParams.waves[i].origin.z,
                        -1.0f,
                        1.0f);

                    glm::vec3& o   = mPlanetParams.waves[i].origin;
                    float      len = glm::length(o);
                    if (len > 1e-6f)
                        o /= len;
                }

                ImGui::PopID();
            }
        }
    }


    // --- Material ---
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
        mPlanetParams.changes.materialChanged |= ImGui::ColorEdit3("Low Color", &mPlanetParams.colorLow.x);
        mPlanetParams.changes.materialChanged |= ImGui::ColorEdit3("Mid Color", &mPlanetParams.color.x);
        mPlanetParams.changes.materialChanged |= ImGui::ColorEdit3("High Color", &mPlanetParams.colorHigh.x);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("Diffuse", &mPlanetParams.pd, 0.0f, 1.0f);
        mPlanetParams.changes.materialChanged |=
            ImGui::SliderFloat("Specular", &mPlanetParams.ps, 0.0f, 1.0f);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat(
            "Shininess",
            &mPlanetParams.ns,
            1.0f,
            256.0f);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("Reflectivity",
                                                                    &mPlanetParams.reflectivity,
                                                                    0.0f,
                                                                    1.0f);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("IOR", &mPlanetParams.ior, 1.0f, 3.0f);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("Transparency",
                                                                    &mPlanetParams.transparency,
                                                                    0.0f,
                                                                    1.0f);
    }

    // --- Noise ---
    if (ImGui::CollapsingHeader("Noise", ImGuiTreeNodeFlags_DefaultOpen)) {
        mPlanetParams.changes.noiseChanged |= ImGui::Checkbox("Show Terrain", &mPlanetParams.showTerrain);

        if (ImGui::SliderInt("Number of Noises", &mPlanetParams.noiseCount, 0, 5)) {
            mPlanetParams.changes.noiseChanged = true;

            while ((int)mPlanetParams.noise.size() < mPlanetParams.noiseCount)
                mPlanetParams.noise.emplace_back();
            while ((int)mPlanetParams.noise.size() > mPlanetParams.noiseCount)
                mPlanetParams.noise.pop_back();
        }

        for (int i = 0; i < mPlanetParams.noiseCount; i++) {
            ImGui::PushID(i);

            std::string name = "Noise " + std::to_string(i);
            if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                const char* noiseTypes[] = { "Normal", "Turbulent", "Ridged" };

                if (ImGui::Combo("Noise Type",
                                 &mPlanetParams.noise[i].type,
                                 noiseTypes,
                                 IM_ARRAYSIZE(noiseTypes)))
                    mPlanetParams.changes.noiseChanged = true;

                mPlanetParams.changes.noiseChanged |= ImGui::Checkbox("Show", &mPlanetParams.noise[i].shown);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderInt("Octaves",
                                                                       &mPlanetParams.noise[i].octaves,
                                                                       0,
                                                                       10);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Roughness",
                    &mPlanetParams.noise[i].roughness,
                    0.0f,
                    5.0f);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Frequency",
                    &mPlanetParams.noise[i].frequency,
                    0.0f,
                    2.0f);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Amplitude",
                    &mPlanetParams.noise[i].amplitude,
                    0.0f,
                    2.0f);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Persistence",
                    &mPlanetParams.noise[i].persistence,
                    0.0f,
                    1.0f);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Offset",
                    &mPlanetParams.noise[i].offset,
                    0.0f,
                    5.0f);
            }

            ImGui::PopID();
        }
    }

    // --- Presets ---
    if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Earthlike"))
            applyPreset(PRESET_EARTHLIKE);
        ImGui::SameLine();
        if (ImGui::Button("Terrestrial"))
            applyPreset(PRESET_TERRESTRIAL);
        ImGui::SameLine();
        if (ImGui::Button("Icy"))
            applyPreset(PRESET_ICY);

        if (ImGui::Button("Barren"))
            applyPreset(PRESET_BARREN);
        ImGui::SameLine();
        if (ImGui::Button("Volcanic"))
            applyPreset(PRESET_VOLCANIC);
        ImGui::SameLine();
        if (ImGui::Button("Gas Giant"))
            applyPreset(PRESET_GAS_GIANT);

        if (ImGui::Button("Ocean"))
            applyPreset(PRESET_OCEAN);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

UI::~UI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}