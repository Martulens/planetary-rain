#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"
#include <GL/freeglut.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

void UI::initUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Set initial display size BEFORE calling Init
    io.DisplaySize = ImVec2((float)glutGet(GLUT_WINDOW_WIDTH),
                            (float)glutGet(GLUT_WINDOW_HEIGHT));

    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 330");
}

void UI::randomize()
{
    mPlanetParams.radius = 1.5f + (rand() / (float)RAND_MAX) * 15.0f;
    //mPlanetParams.detail = 5 + rand() % 50;

    mPlanetParams.color = glm::vec3(
        rand() / (float)RAND_MAX,
        rand() / (float)RAND_MAX,
        rand() / (float)RAND_MAX
    );

    /*
    mPlanetParams.pd = (rand() / (float)RAND_MAX);
    mPlanetParams.ps = (rand() / (float)RAND_MAX);
    mPlanetParams.ns = 1.0f + (rand() / (float)RAND_MAX) * 255.0f;

    bool reflective = (rand() % 2 == 0);
    if (reflective)
        mPlanetParams.reflectivity = (rand() / (float)RAND_MAX);
    else
        mPlanetParams.reflectivity = 0.0f;

    bool refractive = (rand() % 2 == 0);
    if (refractive)
    {
        mPlanetParams.ior = 1.0f + (rand() / (float)RAND_MAX) * 2.0f;
        mPlanetParams.transparency = (rand() / (float)RAND_MAX);
    }
    else
    {
        mPlanetParams.ior = 1.0f;
        mPlanetParams.transparency = 0.0f;
    }
    */

    // Randomize noise
    mPlanetParams.noiseCount = 1 + rand() % 3; // 1 to 3 noise layers
    std::vector<NoiseSettings> newNoises;

    for (int i = 0; i < mPlanetParams.noiseCount; i++) {
        NoiseSettings n;
        n.shown = true;
        n.offset = (rand() / (float)RAND_MAX) * 5.0f;

        if (i == 0) {
            // Continents: low freq, moderate amp
            n.octaves = 2 + rand() % 2;                                      // 2 to 3
            n.frequency = 0.3f + (rand() / (float)RAND_MAX) * 0.3f;          // 0.3 to 0.6
            n.amplitude = 0.5f + (rand() / (float)RAND_MAX) * 0.5f;          // 0.5 to 1.0
            n.persistence = 0.45f + (rand() / (float)RAND_MAX) * 0.15f;      // 0.45 to 0.6
            n.roughness = 1.9f + (rand() / (float)RAND_MAX) * 0.2f;          // 1.9 to 2.1
        }
        else if (i == 1) {
            // Mountains: medium freq, lower amp
            n.octaves = 3 + rand() % 2;                                      // 3 to 4
            n.frequency = 0.6f + (rand() / (float)RAND_MAX) * 0.4f;          // 0.6 to 1.0
            n.amplitude = 0.4f + (rand() / (float)RAND_MAX) * 0.3f;          // 0.4 to 0.7
            n.persistence = 0.5f + (rand() / (float)RAND_MAX) * 0.15f;       // 0.5 to 0.65
            n.roughness = 2.0f + (rand() / (float)RAND_MAX) * 0.2f;          // 2.0 to 2.2
        }
        else {
            // Detail: high freq, low amp
            n.octaves = 3 + rand() % 3;                                      // 3 to 5
            n.frequency = 1.0f + (rand() / (float)RAND_MAX) * 0.5f;          // 1.0 to 1.5
            n.amplitude = 0.2f + (rand() / (float)RAND_MAX) * 0.3f;          // 0.2 to 0.5
            n.persistence = 0.5f + (rand() / (float)RAND_MAX) * 0.2f;        // 0.5 to 0.7
            n.roughness = 2.0f + (rand() / (float)RAND_MAX) * 0.3f;          // 2.0 to 2.3
        }

        newNoises.push_back(n);
    }

    mPlanetParams.noise = newNoises;
    mPlanetParams.changed = true;
}

void UI::renderUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    mPlanetParams.changed = false;
    mParamsChanged = false;

    ImGui::Begin("Planet Editor");

    if (ImGui::Button("Randomize"))
        randomize();

    // Instructions
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Press TAB to toggle mouse");
    ImGui::Separator();

    // Geometry
    if (ImGui::CollapsingHeader("Geometry", ImGuiTreeNodeFlags_DefaultOpen))
    {
        mParamsChanged |= ImGui::SliderFloat("Radius", &mPlanetParams.radius, 0.5f, 50.0f);
        mParamsChanged |= ImGui::SliderInt("Detail", &mPlanetParams.detail, 2, 100); // min 2 to form triangles
        mParamsChanged |= ImGui::SliderFloat("x", &mPlanetParams.x, -100.0f, 150.0f);
        mParamsChanged |= ImGui::SliderFloat("y", &mPlanetParams.y, -100.0f, 150.0f);
        mParamsChanged |= ImGui::SliderFloat("z", &mPlanetParams.z, -100.0f, 150.0f);
    }

    // Appearance
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        mParamsChanged |= ImGui::ColorEdit3("Color", &mPlanetParams.color.x); // already 0-1
        mParamsChanged |= ImGui::SliderFloat("Diffuse", &mPlanetParams.pd, 0.0f, 1.0f);
        mParamsChanged |= ImGui::SliderFloat("Specular", &mPlanetParams.ps, 0.0f, 1.0f);
        mParamsChanged |= ImGui::SliderFloat("Shininess", &mPlanetParams.ns, 1.0f, 256.0f);
        mParamsChanged |= ImGui::SliderFloat("Reflectivity", &mPlanetParams.reflectivity, 0.0f, 1.0f);
        mParamsChanged |= ImGui::SliderFloat("IOR", &mPlanetParams.ior, 1.0f, 3.0f);
        mParamsChanged |= ImGui::SliderFloat("Transparency", &mPlanetParams.transparency, 0.0f, 1.0f);
    }

    // Noise
    if (ImGui::CollapsingHeader("Noise", ImGuiTreeNodeFlags_DefaultOpen))
    {
        mParamsChanged |= ImGui::Checkbox("Show Terrain", &mPlanetParams.showTerrain);

        if (ImGui::SliderInt("Number of Noises", &mPlanetParams.noiseCount, 0, 5))
        {
            mParamsChanged = true;

            while ((int)mPlanetParams.noise.size() < mPlanetParams.noiseCount)
            {
                mPlanetParams.noise.emplace_back();
            }
            while ((int)mPlanetParams.noise.size() > mPlanetParams.noiseCount)
            {
                mPlanetParams.noise.pop_back();
            }
        }

        for (int i = 0; i < mPlanetParams.noiseCount; i++)
        {
            ImGui::PushID(i); // Ensure unique widget IDs

            std::string name = "Noise " + std::to_string(i);
            if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){
                const char* noiseTypes[] = { "Normal", "Turbulent", "Ridged"};

                if (ImGui::Combo("Noise Type", &mPlanetParams.noise[i].type, noiseTypes, IM_ARRAYSIZE(noiseTypes))) {
                    // currentType changed
                    mParamsChanged = true;
                }

                mParamsChanged |= ImGui::Checkbox("Show", &mPlanetParams.noise[i].shown);
                mParamsChanged |= ImGui::SliderInt("Octaves", &mPlanetParams.noise[i].octaves, 0, 10);
                mParamsChanged |= ImGui::SliderFloat("Roughness", &mPlanetParams.noise[i].roughness, 0.0f, 5.0f);
                mParamsChanged |= ImGui::SliderFloat("Frequency", &mPlanetParams.noise[i].frequency, 0.0f, 2.0f);
                mParamsChanged |= ImGui::SliderFloat("Amplitude", &mPlanetParams.noise[i].amplitude, 0.0f, 2.0f);
                mParamsChanged |= ImGui::SliderFloat("Persistence", &mPlanetParams.noise[i].persistence, 0.0f, 1.0f);
                mParamsChanged |= ImGui::SliderFloat("Offset", &mPlanetParams.noise[i].offset, 0.0f, 5.0f);
            }

            ImGui::PopID();
        }
    }

    // Animation
    if (ImGui::CollapsingHeader("Animation"))
    {
        ImGui::Checkbox("Auto Rotate", &mPlanetParams.autoRotate);
        ImGui::SliderFloat("Speed (deg/s)", &mPlanetParams.rotationSpeed, 0.0f, 180.0f);
    }

    // Presets
    if (ImGui::CollapsingHeader("Presets"))
    {
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
            // Continents
            mPlanetParams.noise.push_back({true, 4, 1.0f, 0.15f, 0.5f, 2.0f, 0.0f});
            // Mountains
            mPlanetParams.noise.push_back({true, 6, 2.5f, 0.08f, 0.4f, 2.2f, 42.0f});

            mPlanetParams.changed = true;
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
            // Subtle bands
            mPlanetParams.noise.push_back({true, 2, 0.5f, 0.02f, 0.6f, 1.5f, 0.0f});

            mPlanetParams.changed = true;
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
            // Cracked ice terrain
            mPlanetParams.noise.push_back({true, 3, 1.5f, 0.05f, 0.5f, 2.0f, 10.0f});
            // Fine detail
            mPlanetParams.noise.push_back({true, 5, 3.0f, 0.02f, 0.4f, 2.5f, 77.0f});

            mPlanetParams.changed = true;
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
            // Volcanic terrain
            mPlanetParams.noise.push_back({true, 4, 1.2f, 0.2f, 0.5f, 2.0f, 33.0f});
            // Lava cracks
            mPlanetParams.noise.push_back({true, 6, 2.8f, 0.1f, 0.35f, 2.3f, 99.0f});

            mPlanetParams.changed = true;
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
            // Large features
            mPlanetParams.noise.push_back({true, 3, 0.8f, 0.12f, 0.5f, 2.0f, 5.0f});
            // Craters
            mPlanetParams.noise.push_back({true, 5, 2.0f, 0.08f, 0.45f, 2.1f, 20.0f});
            // Surface roughness
            mPlanetParams.noise.push_back({true, 7, 3.5f, 0.03f, 0.4f, 2.4f, 50.0f});

            mPlanetParams.changed = true;
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
            mPlanetParams.noise.push_back({true, 2, 0.6f, 0.03f, 0.5f, 1.8f, 0.0f});

            mPlanetParams.changed = true;
        }
    }

    ImGui::End();

    mParamsChanged = mPlanetParams.changed || mParamsChanged;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

UI::~UI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

// Forward GLUT events to ImGui
