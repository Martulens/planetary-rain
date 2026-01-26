#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"
#include <GL/freeglut.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

void initUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 330");
}

void renderUI(PlanetParams& params, bool& paramsChanged)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    params.changed = false;
    paramsChanged = false;

    ImGui::Begin("Planet Editor");

    // Instructions
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Press TAB to toggle mouse");
    ImGui::Separator();

    // Geometry
    if (ImGui::CollapsingHeader("Geometry", ImGuiTreeNodeFlags_DefaultOpen)){
        paramsChanged |= ImGui::SliderFloat("Radius", &params.radius, 0.5f, 50.0f);
        paramsChanged |= ImGui::SliderInt("Detail", &params.detail, 2, 100); // min 2 to form triangles
        paramsChanged |= ImGui::SliderFloat("x", &params.x, -100.0f, 150.0f);
        paramsChanged |= ImGui::SliderFloat("y", &params.y, -100.0f, 150.0f);
        paramsChanged |= ImGui::SliderFloat("z", &params.z, -100.0f, 150.0f);
    }

    // Appearance
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)){
        paramsChanged |= ImGui::ColorEdit3("Color", &params.color.x); // already 0-1
        paramsChanged |= ImGui::SliderFloat("Diffuse", &params.pd, 0.0f, 1.0f);
        paramsChanged |= ImGui::SliderFloat("Specular", &params.ps, 0.0f, 1.0f);
        paramsChanged |= ImGui::SliderFloat("Shininess", &params.ns, 1.0f, 256.0f);
        paramsChanged |= ImGui::SliderFloat("Reflectivity", &params.reflectivity, 0.0f, 1.0f);
        paramsChanged |= ImGui::SliderFloat("IOR", &params.ior, 1.0f, 3.0f);
        paramsChanged |= ImGui::SliderFloat("Transparency", &params.transparency, 0.0f, 1.0f);
    }

    // Noise
    if (ImGui::CollapsingHeader("Noise", ImGuiTreeNodeFlags_DefaultOpen)){
        paramsChanged |= ImGui::Checkbox("Show Terrain", &params.showTerrain);

        if (ImGui::SliderInt("Number of Noises", &params.noiseCount, 0, 5))
        {
            paramsChanged = true;

            while ((int)params.noise.size() < params.noiseCount)
            {
                params.noise.emplace_back();
            }
            while ((int)params.noise.size() > params.noiseCount)
            {
                params.noise.pop_back();
            }
        }

        for (int i = 0; i < params.noiseCount; i++)
        {
            ImGui::PushID(i); // Ensure unique widget IDs

            std::string name = "Noise " + std::to_string(i);
            if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){
                paramsChanged |= ImGui::Checkbox("Show", &params.noise[i].shown);
                paramsChanged |= ImGui::SliderInt("Octaves", &params.noise[i].octaves, 0, 10);
                paramsChanged |= ImGui::SliderFloat("Roughness", &params.noise[i].roughness, 0.0f, 5.0f);
                paramsChanged |= ImGui::SliderFloat("Frequency", &params.noise[i].frequency, 0.0f, 5.0f);
                paramsChanged |= ImGui::SliderFloat("Amplitude", &params.noise[i].amplitude, 1.0f, 2.0f);
                paramsChanged |= ImGui::SliderFloat("Persistence", &params.noise[i].persistence, 0.0f, 1.0f);
                paramsChanged |= ImGui::SliderFloat("Offset", &params.noise[i].offset, 0.0f, 100.0f);
            }

            ImGui::PopID();
        }
    }

    // Animation
    if (ImGui::CollapsingHeader("Animation"))
    {
        ImGui::Checkbox("Auto Rotate", &params.autoRotate);
        ImGui::SliderFloat("Speed (deg/s)", &params.rotationSpeed, 0.0f, 180.0f);
    }

    if (ImGui::Button("Randomize"))
    {
        params.radius = 1.5f + (rand() / (float)RAND_MAX) * 15.0f;
        //params.detail = 5 + rand() % 50;

        params.color = glm::vec3(
            rand() / (float)RAND_MAX,
            rand() / (float)RAND_MAX,
            rand() / (float)RAND_MAX
        );

        /*
        params.pd = (rand() / (float)RAND_MAX);
        params.ps = (rand() / (float)RAND_MAX);
        params.ns = 1.0f + (rand() / (float)RAND_MAX) * 255.0f;

        bool reflective = (rand() % 2 == 0);
        if (reflective)
            params.reflectivity = (rand() / (float)RAND_MAX);
        else
            params.reflectivity = 0.0f;

        bool refractive = (rand() % 2 == 0);
        if (refractive)
        {
            params.ior = 1.0f + (rand() / (float)RAND_MAX) * 2.0f;
            params.transparency = (rand() / (float)RAND_MAX);
        }
        else
        {
            params.ior = 1.0f;
            params.transparency = 0.0f;
        }
        */

        // Randomize noise
        //params.noiseCount = 1 + rand() % 3;  // 1 to 3 noise layers
        std::vector<NoiseSettings> newNoises;
        for (int i = 0; i < params.noiseCount; i++) {
            NoiseSettings n;
            n.shown = true;
            n.octaves = 2 + rand() % 5;                                      // 2 to 6
            n.roughness = 0.5f + (rand() / (float)RAND_MAX) * 4.5f;          // 0.5 to 3.0
            n.frequency = 0.8f + (rand() / (float)RAND_MAX) * 4.2f;          // 0.8 to 2.2
            n.amplitude = 1.0f + (rand() / (float)RAND_MAX) * 1.0f;
            n.persistence = 0.35f + (rand() / (float)RAND_MAX) * 0.65f;      // 0.35 to 0.6
            n.offset = (rand() / (float)RAND_MAX) * 100.0f;                  // 0.0 to 100.0
            newNoises.push_back(n);
        }

        params.noise = newNoises;
        params.changed = true;
    }

    // Presets
    if (ImGui::CollapsingHeader("Presets"))
    {
        if (ImGui::Button("Earth-like"))
        {
            params.color = glm::vec3(0.2f, 0.5f, 0.3f);
            params.pd = 0.8f;
            params.ps = 0.2f;
            params.ns = 16.0f;
            params.reflectivity = 0.1f;
            params.ior = 1.0f;
            params.transparency = 0.0f;

            params.noiseCount = 2;
            params.noise.clear();
            // Continents
            params.noise.push_back({true, 4, 1.0f, 0.15f, 0.5f, 2.0f, 0.0f});
            // Mountains
            params.noise.push_back({true, 6, 2.5f, 0.08f, 0.4f, 2.2f, 42.0f});

            params.changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Gas Giant"))
        {
            params.color = glm::vec3(0.8f, 0.6f, 0.4f);
            params.pd = 0.9f;
            params.ps = 0.1f;
            params.ns = 8.0f;
            params.reflectivity = 0.05f;
            params.ior = 1.0f;
            params.transparency = 0.0f;

            params.noiseCount = 1;
            params.noise.clear();
            // Subtle bands
            params.noise.push_back({true, 2, 0.5f, 0.02f, 0.6f, 1.5f, 0.0f});

            params.changed = true;
        }
        if (ImGui::Button("Ice World"))
        {
            params.color = glm::vec3(0.7f, 0.85f, 0.95f);
            params.pd = 0.5f;
            params.ps = 0.8f;
            params.ns = 64.0f;
            params.reflectivity = 0.4f;
            params.ior = 1.0f;
            params.transparency = 0.0f;

            params.noiseCount = 2;
            params.noise.clear();
            // Cracked ice terrain
            params.noise.push_back({true, 3, 1.5f, 0.05f, 0.5f, 2.0f, 10.0f});
            // Fine detail
            params.noise.push_back({true, 5, 3.0f, 0.02f, 0.4f, 2.5f, 77.0f});

            params.changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Molten"))
        {
            params.color = glm::vec3(0.9f, 0.3f, 0.1f);
            params.pd = 0.7f;
            params.ps = 0.6f;
            params.ns = 32.0f;
            params.reflectivity = 0.2f;
            params.ior = 1.0f;
            params.transparency = 0.0f;

            params.noiseCount = 2;
            params.noise.clear();
            // Volcanic terrain
            params.noise.push_back({true, 4, 1.2f, 0.2f, 0.5f, 2.0f, 33.0f});
            // Lava cracks
            params.noise.push_back({true, 6, 2.8f, 0.1f, 0.35f, 2.3f, 99.0f});

            params.changed = true;
        }
        if (ImGui::Button("Rocky"))
        {
            params.color = glm::vec3(0.5f, 0.45f, 0.4f);
            params.pd = 0.9f;
            params.ps = 0.1f;
            params.ns = 8.0f;
            params.reflectivity = 0.0f;
            params.ior = 1.0f;
            params.transparency = 0.0f;

            params.noiseCount = 3;
            params.noise.clear();
            // Large features
            params.noise.push_back({true, 3, 0.8f, 0.12f, 0.5f, 2.0f, 5.0f});
            // Craters
            params.noise.push_back({true, 5, 2.0f, 0.08f, 0.45f, 2.1f, 20.0f});
            // Surface roughness
            params.noise.push_back({true, 7, 3.5f, 0.03f, 0.4f, 2.4f, 50.0f});

            params.changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Smooth"))
        {
            params.color = glm::vec3(0.6f, 0.55f, 0.7f);
            params.pd = 0.6f;
            params.ps = 0.4f;
            params.ns = 32.0f;
            params.reflectivity = 0.15f;
            params.ior = 1.0f;
            params.transparency = 0.0f;

            params.noiseCount = 1;
            params.noise.clear();
            params.noise.push_back({true, 2, 0.6f, 0.03f, 0.5f, 1.8f, 0.0f});

            params.changed = true;
        }
    }

    ImGui::End();

    paramsChanged = params.changed || paramsChanged;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void shutdownUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

// Forward GLUT events to ImGui
void uiKeyboardCallback(unsigned char key, int x, int y)
{
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);
}

void uiMouseCallback(int button, int state, int x, int y)
{
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}

void uiMotionCallback(int x, int y)
{
    ImGui_ImplGLUT_MotionFunc(x, y);
}

void uiReshapeCallback(int w, int h)
{
    ImGui_ImplGLUT_ReshapeFunc(w, h);
}
