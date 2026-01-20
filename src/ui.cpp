#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"
#include <GL/freeglut.h>

void initUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 330");
}

void renderUI(PlanetParams& params, bool& paramsChanged) {
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
    if (ImGui::CollapsingHeader("Geometry", ImGuiTreeNodeFlags_DefaultOpen)) {
        paramsChanged |= ImGui::SliderFloat("Radius", &params.radius, 0.5f, 5.0f);
        paramsChanged |= ImGui::SliderInt("Detail", &params.detail, 1, 4);
        paramsChanged |= ImGui::SliderFloat("x", &params.x, -15.0f, 15.0f);
        paramsChanged |= ImGui::SliderFloat("y", &params.y, -15.0f, 15.0f);
        paramsChanged |= ImGui::SliderFloat("z", &params.z, -15.0f, 15.0f);
    }
    
    // Appearance
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
        paramsChanged |= ImGui::ColorEdit3("Color", &params.color.x);
        paramsChanged |= ImGui::SliderFloat("Diffuse", &params.pd, 0.0f, 1.0f);
        paramsChanged |= ImGui::SliderFloat("Specular", &params.ps, 0.0f, 1.0f);
        paramsChanged |= ImGui::SliderFloat("Shininess", &params.ns, 1.0f, 256.0f);
        paramsChanged |= ImGui::SliderFloat("Reflectivity", &params.reflectivity, 0.0f, 1.0f);
        paramsChanged |= ImGui::SliderFloat("IOR", &params.ns, -1.0f, 5.0f);
        paramsChanged |= ImGui::SliderFloat("Transparency", &params.ns, 0.0f, 1.0f);
    }
    
    // Animation
    if (ImGui::CollapsingHeader("Animation")) {
        ImGui::Checkbox("Auto Rotate", &params.autoRotate);
        ImGui::SliderFloat("Speed (deg/s)", &params.rotationSpeed, 0.0f, 180.0f);
    }
    
    // Presets
    if (ImGui::CollapsingHeader("Presets")) {
        if (ImGui::Button("Earth-like")) {
            params.color = glm::vec3(0.2f, 0.5f, 0.3f);
            params.pd = 0.8f; params.ps = 0.2f; params.ns = 16.0f;
            params.reflectivity = 0.1f;
            params.changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Gas Giant")) {
            params.color = glm::vec3(0.8f, 0.6f, 0.4f);
            params.pd = 0.9f; params.ps = 0.1f; params.ns = 8.0f;
            params.reflectivity = 0.05f;
            params.changed = true;
        }
        if (ImGui::Button("Ice World")) {
            params.color = glm::vec3(0.7f, 0.85f, 0.95f);
            params.pd = 0.5f; params.ps = 0.8f; params.ns = 64.0f;
            params.reflectivity = 0.4f;
            params.changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Molten")) {
            params.color = glm::vec3(0.9f, 0.3f, 0.1f);
            params.pd = 0.7f; params.ps = 0.6f; params.ns = 32.0f;
            params.reflectivity = 0.2f;
            params.changed = true;
        }
    }

    ImGui::End();

    paramsChanged = params.changed || paramsChanged;
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void shutdownUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

// Forward GLUT events to ImGui
void uiKeyboardCallback(unsigned char key, int x, int y) {
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);
}

void uiMouseCallback(int button, int state, int x, int y) {
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}

void uiMotionCallback(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
}

void uiReshapeCallback(int w, int h) {
    ImGui_ImplGLUT_ReshapeFunc(w, h);
}