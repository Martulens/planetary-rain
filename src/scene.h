#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "object.h"
#include "sphere.h"
#include "light.h"
#include "shaderProgram.h"
#include "modelTexture.h"

class Scene {
private:
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    
    // Shared resources
    ShaderProgram* defaultShader = nullptr;

public:
    Scene() = default;
    ~Scene();

    void init();
    void cleanup();

    void constructLights();
    void constructObjects();

    void addObject(Object* obj);
    void addLight(Light* light);

    void update(float deltaTime);

    // === GETTERS
    const std::vector<Object*>& getObjects() const { return objects; }
    const std::vector<Light*>& getLights() const { return lights; }
    ShaderProgram* getDefaultShader() const { return defaultShader; }
};

#endif // SCENE_H