#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "camera.h"
#include "object.h"
#include "sphere.h"
#include "light.h"
#include "shaderProgram.h"
#include "modelTexture.h"
#include "skybox.h"
#include "ui.h"

class Scene {
private:
    std::vector<std::shared_ptr<Object>> mObjects;
    std::vector<std::shared_ptr<Light>> mLights;
    std::shared_ptr<Sphere> mPlanet;

    // Shared resources
    std::shared_ptr<ShaderProgram> mDefaultShader = nullptr;
    std::shared_ptr<ShaderProgram> mRefractiveShader = nullptr;
    std::shared_ptr<Skybox> mSkybox = nullptr;

public:
    Scene() = default;
    ~Scene();

    void init();
    void cleanup();

    // === Render surroundings
    void renderEnvironmentMaps(std::shared_ptr<Camera> camera);
    void renderSceneForEnvMap(const glm::mat4& view, const glm::mat4& projection, std::shared_ptr<Object> excludeObject);

    // === Contructing scene
    void constructLights();
    void constructDebugLights();
    void constructObjects();
    void constructDebugObjects();

    // === Update cycle
    void update(float deltaTime);
    void updatePlanet(const PlanetParams& params);

    // === Addition functions
    void addObject(std::shared_ptr<Object> obj);
    void addLight(std::shared_ptr<Light> light);

    // === GETTERS
    std::vector<std::shared_ptr<Object>> getObjects() const { return mObjects; }
    std::vector<std::shared_ptr<Light>> getLights() const { return mLights; }
    std::shared_ptr<ShaderProgram> getDefaultShader() const { return mDefaultShader; }
    std::shared_ptr<Skybox> getSkybox() const { return mSkybox; }
};

#endif // SCENE_H