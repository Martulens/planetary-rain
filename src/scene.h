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
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;
    
    // Shared resources
    std::shared_ptr<ShaderProgram> defaultShader = nullptr;
    std::shared_ptr<ShaderProgram> refractiveShader = nullptr;
    std::shared_ptr<Skybox> skybox = nullptr;

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
    std::vector<std::shared_ptr<Object>> getObjects() const { return objects; }
    std::vector<std::shared_ptr<Light>> getLights() const { return lights; }
    std::shared_ptr<ShaderProgram> getDefaultShader() const { return defaultShader; }
    std::shared_ptr<Skybox> getSkybox() const { return skybox; }
};

#endif // SCENE_H