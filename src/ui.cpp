#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"
#include <GL/freeglut.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <random>
#include <bits/algorithmfwd.h>

#include "glm/gtx/string_cast.hpp"

void UI::initUI(){
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

void UI::randomize()
{
    enum PlanetType { TERRESTRIAL, EARTHLIKE, DESERT, OCEAN };
    int type = rand() % 4;

    auto rf = [](float lo, float hi)
    {
        return lo + (rand() / (float)RAND_MAX) * (hi - lo);
    };

    auto makeNoise = [](bool shown, int type, int octaves, float frequency, float amplitude, float persistence,
                        float roughness, float offset)
    {
        NoiseSettings n;
        n.shown = shown;
        n.type = type;
        n.octaves = octaves;
        n.frequency = frequency;
        n.amplitude = amplitude + 1;
        n.persistence = persistence;
        n.roughness = roughness;
        n.offset = offset;
        return n;
    };

    mPlanetParams.reflectivity = 0.0f;
    mPlanetParams.ior = 1.0f;
    mPlanetParams.transparency = 0.0f;

    std::cout << "[RANDOMIZE]: PlanetType " << type << std::endl;
    switch (type)
    {
    case TERRESTRIAL:
        {
            float grey = rf(0.35f, 0.55f);
            mPlanetParams.color = glm::vec3(grey, grey * rf(0.9f, 1.0f), grey * rf(0.85f, 0.95f));
            mPlanetParams.pd = rf(0.8f, 1.0f);
            mPlanetParams.ps = rf(0.05f, 0.15f);
            mPlanetParams.ns = rf(4.0f, 16.0f);

            mPlanetParams.noiseCount = 2 + rand() % 2;
            mPlanetParams.noise.clear();
            mPlanetParams.noise.push_back(
                makeNoise(true, 0, 3 + rand() % 2, rf(0.6f, 1.0f), rf(0.08f, 0.15f), rf(0.45f, 0.55f),
                          rf(1.9f, 2.1f), rf(0.0f, 5.0f)));
            mPlanetParams.noise.push_back(
                makeNoise(true, rand() % 2, 4 + rand() % 3, rf(1.5f, 2.5f), rf(0.04f, 0.1f), rf(0.4f, 0.5f),
                          rf(2.0f, 2.3f), rf(0.0f, 5.0f)));
            if (mPlanetParams.noiseCount == 3)
                mPlanetParams.noise.push_back(
                    makeNoise(true, 0, 5 + rand() % 3, rf(3.0f, 4.0f), rf(0.01f, 0.04f), rf(0.35f, 0.45f),
                              rf(2.2f, 2.5f), rf(0.0f, 5.0f)));
            break;
        }
    case EARTHLIKE:
        {
            mPlanetParams.color = glm::vec3(rf(0.15f, 0.35f), rf(0.35f, 0.6f), rf(0.2f, 0.4f));
            mPlanetParams.pd = rf(0.7f, 0.9f);
            mPlanetParams.ps = rf(0.1f, 0.3f);
            mPlanetParams.ns = rf(12.0f, 32.0f);
            mPlanetParams.reflectivity = rf(0.05f, 0.15f);

            mPlanetParams.noiseCount = 2 + rand() % 2;
            mPlanetParams.noise.clear();
            mPlanetParams.noise.push_back(
                makeNoise(true, 0, 3 + rand() % 2, rf(0.4f, 0.8f), rf(0.1f, 0.2f), rf(0.45f, 0.55f),
                          rf(1.9f, 2.1f), rf(0.0f, 5.0f)));
            mPlanetParams.noise.push_back(
                makeNoise(true, rand() % 2, 4 + rand() % 3, rf(1.5f, 2.5f), rf(0.05f, 0.1f), rf(0.4f, 0.5f),
                          rf(2.0f, 2.3f), rf(0.0f, 5.0f)));
            if (mPlanetParams.noiseCount == 3)
                mPlanetParams.noise.push_back(
                    makeNoise(true, 0, 5 + rand() % 2, rf(3.0f, 4.0f), rf(0.01f, 0.03f), rf(0.35f, 0.45f),
                              rf(2.1f, 2.4f), rf(0.0f, 5.0f)));
            break;
        }
    case DESERT:
        {
            mPlanetParams.color = glm::vec3(rf(0.7f, 0.9f), rf(0.5f, 0.7f), rf(0.3f, 0.45f));
            mPlanetParams.pd = rf(0.85f, 1.0f);
            mPlanetParams.ps = rf(0.02f, 0.1f);
            mPlanetParams.ns = rf(4.0f, 12.0f);

            mPlanetParams.noiseCount = 2 + rand() % 2;
            mPlanetParams.noise.clear();
            mPlanetParams.noise.push_back(
                makeNoise(true, 0, 3 + rand() % 2, rf(0.5f, 0.9f), rf(0.06f, 0.12f), rf(0.5f, 0.6f),
                          rf(1.9f, 2.1f), rf(0.0f, 5.0f)));
            mPlanetParams.noise.push_back(
                makeNoise(true, 2, 4 + rand() % 2, rf(1.2f, 2.0f), rf(0.05f, 0.1f), rf(0.4f, 0.55f),
                          rf(2.0f, 2.3f), rf(0.0f, 5.0f)));
            if (mPlanetParams.noiseCount == 3)
                mPlanetParams.noise.push_back(
                    makeNoise(true, 0, 5 + rand() % 3, rf(2.5f, 3.5f), rf(0.01f, 0.03f), rf(0.35f, 0.45f),
                              rf(2.1f, 2.4f), rf(0.0f, 5.0f)));
            break;
        }
    case OCEAN:
        {
            mPlanetParams.color = glm::vec3(rf(0.05f, 0.2f), rf(0.2f, 0.45f), rf(0.5f, 0.8f));
            mPlanetParams.pd = rf(0.5f, 0.7f);
            mPlanetParams.ps = rf(0.4f, 0.7f);
            mPlanetParams.ns = rf(32.0f, 96.0f);
            mPlanetParams.reflectivity = rf(0.3f, 0.6f);
            mPlanetParams.transparency = rf(0.05f, 0.15f);
            mPlanetParams.ior = rf(1.3f, 1.4f);

            mPlanetParams.noiseCount = 1 + rand() % 2;
            mPlanetParams.noise.clear();
            mPlanetParams.noise.push_back(
                makeNoise(true, 0, 2 + rand() % 2, rf(0.4f, 0.8f), rf(0.02f, 0.05f), rf(0.5f, 0.6f),
                          rf(1.8f, 2.0f), rf(0.0f, 3.0f)));
            if (mPlanetParams.noiseCount == 2)
                mPlanetParams.noise.push_back(
                    makeNoise(true, 0, 3 + rand() % 2, rf(1.5f, 2.5f), rf(0.01f, 0.02f), rf(0.4f, 0.5f),
                              rf(2.0f, 2.2f), rf(0.0f, 3.0f)));
            break;
        }
    }

    // Randomize changes material + noise, not mesh detail
    mPlanetParams.changes.materialChanged = true;
    mPlanetParams.changes.noiseChanged = true;
    mPlanetParams.changes.radiusChanged = true;
}

// Realistic randomization for spherical Gerstner waves
void randomizeWaves(PlanetParams& params, int count = 5)
{
    std::mt19937 rng(std::random_device{}());

    params.wavesEnabled = true;
    params.waveCount = count;
    params.waveFadeE0 = 0.01f;
    params.waveFadeE1 = 0.1f;
    params.waves.clear();

    //Distribution helpers
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> cosThetaDist(-1.0f, 1.0f);

    //Each successive wave : smaller amplitude, higher frequency(detail cascades)
    for (int i = 0; i < count; i++){
        WaveSettings set;

        //Amplitude decreases with each wave layer
        // Primary swell is biggest, secondary chop is smaller
        float baseAmp = 3.0f * std::pow(0.55f, static_cast<float>(i));
        std::uniform_real_distribution<float> ampJitter(0.8f, 1.2f);
        float val = baseAmp * ampJitter(rng);
        set.amplitude = glm::clamp(val, 0.01f, 0.5f);

        // Frequency increases with each layer(shorter wavelengthsfor detail)
        float baseFreq = 0.05f * std::pow(1.8f, static_cast<float>(i));
        std::uniform_real_distribution<float> freqJitter(0.85f, 7.5f);
        set.frequency = glm::clamp(baseFreq * freqJitter(rng), 0.5f, 3.5f);

        // Speed(phase velocity) - bigger
        float baseSpeed = 0.15f * std::pow(0.7f, static_cast<float>(i));
        std::uniform_real_distribution<float> speedJitter(1.8f, 5.0f);
        set.speed = glm::clamp(baseSpeed * speedJitter(rng), 1.8f, 5.0f);

        //Steepness Q :
        // 0 = pure sine, 1 = sharp Gerstner peaks
        // Keep well below 1.0 to avoid looping artifacts
        float baseSteep = 0.3f + 0.15f * i;
        std::uniform_real_distribution<float> steepJitter(0.9f, 1.1f);
        set.steepness = glm::clamp(baseSteep * steepJitter(rng), 0.1f, 0.75f);

        // Origin:
        // random point on unit sphere(the shader expects a direction)
        // Using uniform sphere sampling
        float cosTheta = cosThetaDist(rng);
        float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
        float phi = angleDist(rng);
        set.origin = glm::vec3(
            sinTheta * std::cos(phi),
            sinTheta * std::sin(phi),
            cosTheta
        );

        params.waves.emplace_back(set);
    }
}

void UI::renderUI()
{
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
    if (ImGui::CollapsingHeader("Geometry", ImGuiTreeNodeFlags_DefaultOpen))
    {
        mPlanetParams.changes.radiusChanged |= ImGui::SliderFloat("Radius", &mPlanetParams.radius, 0.5f, 50.0f);
        mPlanetParams.changes.meshChanged |= ImGui::SliderInt("Detail", &mPlanetParams.detail, 1, 500);
        mPlanetParams.changes.positionChanged |= ImGui::SliderFloat("x", &mPlanetParams.x, -100.0f, 150.0f);
        mPlanetParams.changes.positionChanged |= ImGui::SliderFloat("y", &mPlanetParams.y, -100.0f, 150.0f);
        mPlanetParams.changes.positionChanged |= ImGui::SliderFloat("z", &mPlanetParams.z, -100.0f, 150.0f);
    }

    // Animation
    if (ImGui::CollapsingHeader("Animation"))
    {
        mPlanetParams.changes.animationChanged |= ImGui::Checkbox("Auto Rotate", &mPlanetParams.autoRotate);
        mPlanetParams.changes.animationChanged |= ImGui::SliderFloat("Speed (deg/s)", &mPlanetParams.rotationSpeed,
                                                                     0.0f, 180.0f);

        if (!mPlanetParams.autoRotate)
        {
            mPlanetParams.changes.animationChanged |= ImGui::SliderFloat(
                "Angle", &mPlanetParams.rotationAngle, 0.0f, 360.0f);
        }
    }

    // --- Waves ---
    if (ImGui::CollapsingHeader("Waves"))
    {
        mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat("Ocean Level", &mPlanetParams.oceanLevel, 0.0f, 2.0f);
        mPlanetParams.changes.wavesChanged |= ImGui::Checkbox("Enable Waves", &mPlanetParams.wavesEnabled);

        if (mPlanetParams.wavesEnabled)
        {
            if (ImGui::Button("Randomize Waves")){
                 randomizeWaves(mPlanetParams);
                 mPlanetParams.changes.wavesChanged = true;
            }

            mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat("Wave FadeE0", &mPlanetParams.waveFadeE0, 0.001f,
                                                                     0.05f);
            mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat("Wave FadeE1", &mPlanetParams.waveFadeE1, 0.01f,
                                                                     0.5f);

            mPlanetParams.changes.wavesChanged |= ImGui::SliderInt("Number of Waves", &mPlanetParams.waveCount, 0, 10);

            while ((int)mPlanetParams.waves.size() < mPlanetParams.waveCount)
                mPlanetParams.waves.emplace_back();
            while ((int)mPlanetParams.waves.size() > mPlanetParams.waveCount)
                mPlanetParams.waves.pop_back();

            for (int i = 0; i < mPlanetParams.waveCount; i++)
            {
                ImGui::PushID(i);

                std::string name = "Wave " + std::to_string(i);
                if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Amplitude", &mPlanetParams.waves[i].amplitude, 0.01f, 1.0f);
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Frequency", &mPlanetParams.waves[i].frequency, 0.01f, 3.5f);
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Speed", &mPlanetParams.waves[i].speed, 0.0f, 5.0f);
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Steepness", &mPlanetParams.waves[i].steepness, 0.0f, 0.85f);

                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Origin X", &mPlanetParams.waves[i].origin.x, -1.0f, 1.0f);
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Origin Y", &mPlanetParams.waves[i].origin.y, -1.0f, 1.0f);
                    mPlanetParams.changes.wavesChanged |= ImGui::SliderFloat(
                        "Origin Z", &mPlanetParams.waves[i].origin.z, -1.0f, 1.0f);

                    glm::vec3& o = mPlanetParams.waves[i].origin;
                    float len = glm::length(o);
                    if (len > 1e-6f) o /= len;
                }

                ImGui::PopID();
            }
        }
    }


    // --- Material ---
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        mPlanetParams.changes.materialChanged |= ImGui::ColorEdit3("Color", &mPlanetParams.color.x);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("Diffuse", &mPlanetParams.pd, 0.0f, 1.0f);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("Specular", &mPlanetParams.ps, 0.0f, 1.0f);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("Shininess", &mPlanetParams.ns, 1.0f, 256.0f);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("Reflectivity", &mPlanetParams.reflectivity, 0.0f,
                                                                    1.0f);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("IOR", &mPlanetParams.ior, 1.0f, 3.0f);
        mPlanetParams.changes.materialChanged |= ImGui::SliderFloat("Transparency", &mPlanetParams.transparency, 0.0f,
                                                                    1.0f);
    }

    // --- Noise ---
    if (ImGui::CollapsingHeader("Noise", ImGuiTreeNodeFlags_DefaultOpen))
    {
        mPlanetParams.changes.noiseChanged |= ImGui::Checkbox("Show Terrain", &mPlanetParams.showTerrain);

        if (ImGui::SliderInt("Number of Noises", &mPlanetParams.noiseCount, 0, 5))
        {
            mPlanetParams.changes.noiseChanged = true;

            while ((int)mPlanetParams.noise.size() < mPlanetParams.noiseCount)
                mPlanetParams.noise.emplace_back();
            while ((int)mPlanetParams.noise.size() > mPlanetParams.noiseCount)
                mPlanetParams.noise.pop_back();
        }

        for (int i = 0; i < mPlanetParams.noiseCount; i++)
        {
            ImGui::PushID(i);

            std::string name = "Noise " + std::to_string(i);
            if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                const char* noiseTypes[] = {"Normal", "Turbulent", "Ridged"};

                if (ImGui::Combo("Noise Type", &mPlanetParams.noise[i].type, noiseTypes, IM_ARRAYSIZE(noiseTypes)))
                    mPlanetParams.changes.noiseChanged = true;

                mPlanetParams.changes.noiseChanged |= ImGui::Checkbox("Show", &mPlanetParams.noise[i].shown);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderInt("Octaves", &mPlanetParams.noise[i].octaves, 0,
                                                                       10);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Roughness", &mPlanetParams.noise[i].roughness, 0.0f, 5.0f);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Frequency", &mPlanetParams.noise[i].frequency, 0.0f, 2.0f);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Amplitude", &mPlanetParams.noise[i].amplitude, 0.0f, 2.0f);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Persistence", &mPlanetParams.noise[i].persistence, 0.0f, 1.0f);
                mPlanetParams.changes.noiseChanged |= ImGui::SliderFloat(
                    "Offset", &mPlanetParams.noise[i].offset, 0.0f, 5.0f);
            }

            ImGui::PopID();
        }
    }

    // --- Presets ---
    if (ImGui::CollapsingHeader("Presets"))
    {
        auto setPresetFlags = [this]()
        {
            mPlanetParams.changes.materialChanged = true;
            mPlanetParams.changes.noiseChanged = true;
        };

        if (ImGui::Button("Earth-like"))
        {
            mPlanetParams.color = glm::vec3(0.2f, 0.5f, 0.3f);
            mPlanetParams.pd = 0.8f;
            mPlanetParams.ps = 0.2f;
            mPlanetParams.ns = 16.0f;
            mPlanetParams.reflectivity = 0.1f;
            mPlanetParams.ior = 1.0f;
            mPlanetParams.transparency = 0.0f;
            mPlanetParams.noiseCount = 2;
            mPlanetParams.noise.clear();
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 4;
                n.frequency = 1.0f;
                n.amplitude = 0.15f;
                n.persistence = 0.5f;
                n.roughness = 2.0f;
                n.offset = 0.0f;
                mPlanetParams.noise.push_back(n);
            }
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 6;
                n.frequency = 2.5f;
                n.amplitude = 0.08f;
                n.persistence = 0.4f;
                n.roughness = 2.2f;
                n.offset = 42.0f;
                mPlanetParams.noise.push_back(n);
            }
            setPresetFlags();
        }
        ImGui::SameLine();
        if (ImGui::Button("Gas Giant"))
        {
            mPlanetParams.color = glm::vec3(0.8f, 0.6f, 0.4f);
            mPlanetParams.pd = 0.9f;
            mPlanetParams.ps = 0.1f;
            mPlanetParams.ns = 8.0f;
            mPlanetParams.reflectivity = 0.05f;
            mPlanetParams.ior = 1.0f;
            mPlanetParams.transparency = 0.0f;
            mPlanetParams.noiseCount = 1;
            mPlanetParams.noise.clear();
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 2;
                n.frequency = 0.5f;
                n.amplitude = 0.02f;
                n.persistence = 0.6f;
                n.roughness = 1.5f;
                n.offset = 0.0f;
                mPlanetParams.noise.push_back(n);
            }
            setPresetFlags();
        }
        if (ImGui::Button("Ice World"))
        {
            mPlanetParams.color = glm::vec3(0.7f, 0.85f, 0.95f);
            mPlanetParams.pd = 0.5f;
            mPlanetParams.ps = 0.8f;
            mPlanetParams.ns = 64.0f;
            mPlanetParams.reflectivity = 0.4f;
            mPlanetParams.ior = 1.0f;
            mPlanetParams.transparency = 0.0f;
            mPlanetParams.noiseCount = 2;
            mPlanetParams.noise.clear();
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 3;
                n.frequency = 1.5f;
                n.amplitude = 0.05f;
                n.persistence = 0.5f;
                n.roughness = 2.0f;
                n.offset = 10.0f;
                mPlanetParams.noise.push_back(n);
            }
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 5;
                n.frequency = 3.0f;
                n.amplitude = 0.02f;
                n.persistence = 0.4f;
                n.roughness = 2.5f;
                n.offset = 77.0f;
                mPlanetParams.noise.push_back(n);
            }
            setPresetFlags();
        }
        ImGui::SameLine();
        if (ImGui::Button("Molten"))
        {
            mPlanetParams.color = glm::vec3(0.9f, 0.3f, 0.1f);
            mPlanetParams.pd = 0.7f;
            mPlanetParams.ps = 0.6f;
            mPlanetParams.ns = 32.0f;
            mPlanetParams.reflectivity = 0.2f;
            mPlanetParams.ior = 1.0f;
            mPlanetParams.transparency = 0.0f;
            mPlanetParams.noiseCount = 2;
            mPlanetParams.noise.clear();
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 4;
                n.frequency = 1.2f;
                n.amplitude = 0.2f;
                n.persistence = 0.5f;
                n.roughness = 2.0f;
                n.offset = 33.0f;
                mPlanetParams.noise.push_back(n);
            }
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 6;
                n.frequency = 2.8f;
                n.amplitude = 0.1f;
                n.persistence = 0.35f;
                n.roughness = 2.3f;
                n.offset = 99.0f;
                mPlanetParams.noise.push_back(n);
            }
            setPresetFlags();
        }
        if (ImGui::Button("Rocky"))
        {
            mPlanetParams.color = glm::vec3(0.5f, 0.45f, 0.4f);
            mPlanetParams.pd = 0.9f;
            mPlanetParams.ps = 0.1f;
            mPlanetParams.ns = 8.0f;
            mPlanetParams.reflectivity = 0.0f;
            mPlanetParams.ior = 1.0f;
            mPlanetParams.transparency = 0.0f;
            mPlanetParams.noiseCount = 3;
            mPlanetParams.noise.clear();
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 3;
                n.frequency = 0.8f;
                n.amplitude = 0.12f;
                n.persistence = 0.5f;
                n.roughness = 2.0f;
                n.offset = 5.0f;
                mPlanetParams.noise.push_back(n);
            }
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 5;
                n.frequency = 2.0f;
                n.amplitude = 0.08f;
                n.persistence = 0.45f;
                n.roughness = 2.1f;
                n.offset = 20.0f;
                mPlanetParams.noise.push_back(n);
            }
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 7;
                n.frequency = 3.5f;
                n.amplitude = 0.03f;
                n.persistence = 0.4f;
                n.roughness = 2.4f;
                n.offset = 50.0f;
                mPlanetParams.noise.push_back(n);
            }
            setPresetFlags();
        }
        ImGui::SameLine();
        if (ImGui::Button("Smooth"))
        {
            mPlanetParams.color = glm::vec3(0.6f, 0.55f, 0.7f);
            mPlanetParams.pd = 0.6f;
            mPlanetParams.ps = 0.4f;
            mPlanetParams.ns = 32.0f;
            mPlanetParams.reflectivity = 0.15f;
            mPlanetParams.ior = 1.0f;
            mPlanetParams.transparency = 0.0f;
            mPlanetParams.noiseCount = 1;
            mPlanetParams.noise.clear();
            {
                NoiseSettings n;
                n.shown = true;
                n.octaves = 2;
                n.frequency = 0.6f;
                n.amplitude = 0.03f;
                n.persistence = 0.5f;
                n.roughness = 1.8f;
                n.offset = 0.0f;
                mPlanetParams.noise.push_back(n);
            }
            setPresetFlags();
        }
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

UI::~UI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}
