#include "scene.h"
#include "camera.h"
#include "gamestate.h"
#include "config.h"
#include "framework.h"
#include <iostream>


Scene::~Scene() {
    cleanup();
}

void Scene::constructObjects(){
    std::cout << "[Scene] Initializing objects..." << std::endl;

    glm::vec3 redColor(1.0f, 0.2f, 0.2f);
    ModelTexture* matteRedMaterial = new ModelTexture(
        redColor,
        0.9f,
        0.1f,
        8.0f,
        0.0f
    );

    Sphere* sphere1 = new Sphere(
        glm::vec3(-4.0f, 1.5f, 0.0f),
        1.5f,
        3,
        matteRedMaterial,
        defaultShader
    );
    addObject(sphere1);

    glm::vec3 greenColor(0.2f, 0.8f, 0.3f);
    ModelTexture* glossyGreenMaterial = new ModelTexture(
        greenColor,
        0.6f,
        0.5f,
        32.0f,
        0.2f
    );

    Sphere* sphere2 = new Sphere(
        glm::vec3(0.0f, 1.5f, 0.0f),
        1.5f,
        3,
        glossyGreenMaterial,
        defaultShader
    );
    addObject(sphere2);

    glm::vec3 blueColor(0.2f, 0.4f, 1.0f);
    ModelTexture* shinyBlueMaterial = new ModelTexture(
        blueColor,
        0.3f,
        0.9f,
        128.0f,
        0.5f
    );

    Sphere* sphere3 = new Sphere(
        glm::vec3(4.0f, 1.5f, 0.0f),
        1.5f,
        3,
        shinyBlueMaterial,
        defaultShader
    );
    addObject(sphere3);
}

void Scene::constructLights(){
    std::cout << "[Scene] Initializing lights..." << std::endl;

    Light* sunLight = new Light(
        glm::vec3(1.0f, -1.0f, -0.5f),
        glm::vec3(1.0f, 0.95f, 0.9f),
        1.0f,
        false
    );
    addLight(sunLight);

    Light* pointLight1 = new Light(
        glm::vec3(0.0f, 3.0f, 5.0f),
        glm::vec3(1.0f, 0.8f, 0.6f),
        1.5f,
        true
    );
    addLight(pointLight1);

    Light* fillLight = new Light(
        glm::vec3(-5.0f, 2.0f, 2.0f),
        glm::vec3(0.6f, 0.7f, 1.0f),
        0.5f,
        true
    );
    addLight(fillLight);
}

void Scene::init() {
    std::cout << "[Scene] Initializing scene..." << std::endl;

    // Create the default shader
    defaultShader = new ShaderProgram("shaders/template.vert", "shaders/template.frag");

    if (!defaultShader || defaultShader->getProgram() == 0) {
        std::cerr << "[Scene] ERROR: Failed to create default shader!" << std::endl;
        return;
    }

    std::cout << "[Scene] Initializing skybox..." << std::endl;
    int numSkybox = rand() % 5;
    skybox = new Skybox("shaders/skybox.vert", "shaders/skybox.frag", "textures/skybox/" + std::to_string(numSkybox) + "/");
    CHECK_GL_ERROR();

    constructObjects();
    CHECK_GL_ERROR();

    constructLights();
    CHECK_GL_ERROR();

    std::cout << "[Scene] Created " << objects.size() << " objects and "
              << lights.size() << " lights" << std::endl;
}

void Scene::cleanup() {
    // Clean up objects and their associated geometry
    for (Object* obj : objects) {
        if (obj) {
            // Delete the geometry
            if (obj->getGeometry()) {
                delete obj->getGeometry();
            }
            // Delete the texture
            delete obj->getTexture();
            delete obj;
        }
    }
    objects.clear();

    for (Light* light : lights) {
        delete light;
    }
    lights.clear();

    if (defaultShader) {
        delete defaultShader;
        defaultShader = nullptr;
    }

    if (skybox) {
        delete skybox;
        skybox = nullptr;
    }
}

void Scene::addObject(Object* obj) {
    if (obj) {
        objects.push_back(obj);
    }
}

void Scene::addLight(Light* light) {
    if (light) {
        lights.push_back(light);
    }
}

void Scene::update(float deltaTime) {
    static float angle = 0.0f;
    angle += deltaTime * 0.5f;

    // animate sphere positions or rotations
    // for (Object* obj : objects) {
    //     obj->setRotationY(angle * 30.0f);
    // }
}