#include "scene.h"
#include "camera.h"
#include "gamestate.h"
#include "config.h"
#include "framework.h"
#include <iostream>

// FIXED: Removed duplicate "namespace con { Scene* scene = nullptr; }"
// The scene pointer is now defined in context.cpp

Scene::~Scene() {
    cleanup();
}

void Scene::init() {
    std::cout << "[Scene] Initializing scene..." << std::endl;

    // Create the default shader
    defaultShader = new ShaderProgram("shaders/template.vert", "shaders/template.frag");

    if (!defaultShader || defaultShader->getProgram() == 0) {
        std::cerr << "[Scene] ERROR: Failed to create default shader!" << std::endl;
        return;
    }

    // --- Sphere 1: Red, matte (high diffuse, low specular) ---
    glm::vec3 redColor(1.0f, 0.2f, 0.2f);
    ModelTexture* matteRedMaterial = new ModelTexture(
        redColor,
        0.9f,   // pd - high diffuse
        0.1f,   // ps - low specular
        8.0f,   // ns - low shininess
        0.0f    // reflectivity
    );

    Sphere* sphere1 = new Sphere(
        glm::vec3(-4.0f, 1.5f, 0.0f),  // position (left)
        1.5f,                           // radius
        3,                              // detail level
        matteRedMaterial,
        defaultShader
    );
    addObject(sphere1);

    // --- Sphere 2: Green, semi-glossy (balanced diffuse/specular) ---
    glm::vec3 greenColor(0.2f, 0.8f, 0.3f);
    ModelTexture* glossyGreenMaterial = new ModelTexture(
        greenColor,
        0.6f,   // pd - medium diffuse
        0.5f,   // ps - medium specular
        32.0f,  // ns - medium shininess
        0.2f    // reflectivity
    );

    Sphere* sphere2 = new Sphere(
        glm::vec3(0.0f, 1.5f, 0.0f),   // position (center)
        1.5f,                           // radius
        3,                              // detail level
        glossyGreenMaterial,
        defaultShader
    );
    addObject(sphere2);

    // --- Sphere 3: Blue, shiny/metallic (low diffuse, high specular) ---
    glm::vec3 blueColor(0.2f, 0.4f, 1.0f);
    ModelTexture* shinyBlueMaterial = new ModelTexture(
        blueColor,
        0.3f,   // pd - low diffuse
        0.9f,   // ps - high specular
        128.0f, // ns - high shininess (sharp highlights)
        0.5f    // reflectivity
    );

    Sphere* sphere3 = new Sphere(
        glm::vec3(4.0f, 1.5f, 0.0f),   // position (right)
        1.5f,                           // radius
        3,                              // detail level
        shinyBlueMaterial,
        defaultShader
    );
    addObject(sphere3);

    // Main directional light (sun-like, from above-right)
    Light* sunLight = new Light(
        glm::vec3(1.0f, -1.0f, -0.5f),  // direction (pointing down and to the left)
        glm::vec3(1.0f, 0.95f, 0.9f),   // warm white color
        1.0f,                            // brightness
        false                            // NOT a point light (directional)
    );
    addLight(sunLight);

    // Point light (warm, in front of spheres)
    Light* pointLight1 = new Light(
        glm::vec3(0.0f, 3.0f, 5.0f),    // position
        glm::vec3(1.0f, 0.8f, 0.6f),    // warm orange color
        1.5f,                            // brightness
        true                             // IS a point light
    );
    addLight(pointLight1);

    // Fill light (cool, from the left)
    Light* fillLight = new Light(
        glm::vec3(-5.0f, 2.0f, 2.0f),   // position
        glm::vec3(0.6f, 0.7f, 1.0f),    // cool blue color
        0.5f,                            // lower brightness
        true                             // IS a point light
    );
    addLight(fillLight);

    std::cout << "[Scene] Created " << objects.size() << " objects and "
              << lights.size() << " lights" << std::endl;
}

void Scene::cleanup() {
    // Clean up objects and their associated geometry
    for (Object* obj : objects) {
        if (obj) {
            // Delete the geometry (prevents memory leak)
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
    angle += deltaTime * 0.5f;  // Slow rotation

    // Optional: animate sphere positions or rotations
    // for (Object* obj : objects) {
    //     obj->setRotationY(angle * 30.0f);
    // }
}