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
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    
    // Shared resources
    ShaderProgram* defaultShader = nullptr;
    ShaderProgram* refractiveShader = nullptr;
    Skybox* skybox = nullptr;
    Sphere* planet = nullptr;

public:
    Scene() = default;
    ~Scene();

    void init();
    void cleanup();

    void renderEnvironmentMaps(Camera* camera);
    void renderSceneForEnvMap(const glm::mat4& view, const glm::mat4& projection, Object* excludeObject);

    void constructLights();
    void constructDebugLights();
    void constructObjects();
    void constructDebugObjects();

    void addObject(Object* obj);
    void addLight(Light* light);

    void update(float deltaTime);
    void updatePlanet(const PlanetParams& params);

    // === GETTERS
    const std::vector<Object*>& getObjects() const { return objects; }
    const std::vector<Light*>& getLights() const { return lights; }
    ShaderProgram* getDefaultShader() const { return defaultShader; }
    Skybox* getSkybox() const { return skybox; }
};

#endif // SCENE_H