#include "scene.h"
#include "camera.h"
#include "gamestate.h"
#include "config.h"
#include "framework.h"
#include <iostream>
#include <algorithm>

Scene::~Scene(){
    cleanup();
}

void Scene::updatePlanet(const PlanetParams& params){
    if (!planet) return;

    auto it = std::find(objects.begin(), objects.end(), planet);
    if (it != objects.end()) {
        objects.erase(it);
    }

    bool needsEnv = planet->getNeedsEnvMap();

    if (planet->getGeometry()) delete planet->getGeometry();
    if (planet->getTexture()) delete planet->getTexture();
    delete planet;
    planet = nullptr;

    ModelTexture* newMaterial = new ModelTexture(
        params.color,
        params.pd,
        params.ps,
        params.ns,
        params.reflectivity,
        params.ior,
        params.transparency
    );

    ShaderProgram* shader = defaultShader;
    if (params.ior > 1.0f) {
        shader = refractiveShader;
    }

    glm::vec3 newPos(params.x, params.y, params.z);
    planet = new Sphere(newPos, params.radius, params.detail, newMaterial, shader);
    planet->setNeedsEnvMap(needsEnv);

    addObject(planet);
}

void Scene::renderSceneForEnvMap(const glm::mat4& view, const glm::mat4& projection, Object* excludeObject){
    if (skybox)
        skybox->draw(view, projection);

    // Render all objects except the excluded one
    for (Object* obj : objects)
    {
        if (obj == excludeObject) continue;
        if (!obj || !obj->getGeometry() || !obj->getShader()) continue;

        ShaderProgram* shader = obj->getShader();
        glUseProgram(shader->getProgram());

        glUniformMatrix4fv(shader->getModelMatrixLocation(), 1, GL_FALSE, &obj->getModelMatrix()[0][0]);
        glUniformMatrix4fv(shader->getViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shader->getProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(projection));

        // Simplified uniforms for env map rendering
        ModelTexture* texture = obj->getTexture();
        glUniform3fv(shader->getBaseColorLocation(), 1, glm::value_ptr(texture->getColor()));
        glUniform1f(shader->getPdLocation(), texture->getPd());
        glUniform1f(shader->getPsLocation(), texture->getPs());
        glUniform1f(shader->getNsLocation(), texture->getNs());
        glUniform3fv(shader->getSkyColorLocation(), 1, glm::value_ptr(skyColorConst));

        // Set up lights
        glUniform1i(glGetUniformLocation(shader->getProgram(), "numLights"), static_cast<int>(lights.size()));
        for (size_t i = 0; i < lights.size() && i < LIGHTS_MAX; ++i)
        {
            Light* light = lights[i];
            std::string base = "lightPositions[" + std::to_string(i) + "]";
            glUniform3fv(glGetUniformLocation(shader->getProgram(), base.c_str()), 1,
                         glm::value_ptr(light->getPosition()));

            base = "lightColors[" + std::to_string(i) + "]";
            glUniform3fv(glGetUniformLocation(shader->getProgram(), base.c_str()), 1,
                         glm::value_ptr(light->getColor()));

            base = "lightBrightness[" + std::to_string(i) + "]";
            glUniform1f(glGetUniformLocation(shader->getProgram(), base.c_str()), light->getBrightness());

            base = "isPointLight[" + std::to_string(i) + "]";
            glUniform1i(glGetUniformLocation(shader->getProgram(), base.c_str()), light->isPointLight() ? 1 : 0);
        }

        // Disable environment mapping for env map rendering (avoid recursion)
        glUniform1i(glGetUniformLocation(shader->getProgram(), "useEnvironmentLighting"), 0);

        obj->draw();
        glUseProgram(0);
    }
}

void Scene::renderEnvironmentMaps(Camera* camera)
{
    // Store original viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    for (Object* obj : objects)
    {
        if (!obj->getNeedsEnvMap()) continue;

        EnvMap* envMap = obj->getEnvMap();
        if (!envMap)
        {
            obj->createEnvMap(128);
            envMap = obj->getEnvMap();
        }

        // Update view matrices from object's position
        envMap->updateViewMatrices(obj->getPosition());

        // Render all 6 faces
        for (int face = 0; face < 6; ++face)
        {
            envMap->beginCapture(face);

            glClearColor(skyColorConst.r, skyColorConst.g, skyColorConst.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderSceneForEnvMap(envMap->getViewMatrix(face), envMap->getProjection(), obj);

            envMap->endCapture();
        }
    }

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void Scene::constructDebugObjects()
{
    std::cout << "[Scene] Initializing objects..." << std::endl;

    // Red sphere - matte, no reflections needed
    glm::vec3 redColor(1.0f, 0.2f, 0.2f);
    ModelTexture* matteRedMaterial = new ModelTexture(redColor, 0.9f, 0.1f, 8.0f, 0.0f);
    Sphere* sphere1 = new Sphere(glm::vec3(-4.0f, 1.5f, 0.0f), 1.5f, 3, matteRedMaterial, defaultShader);

    sphere1->setNeedsEnvMap(false);
    addObject(sphere1);

    // Green sphere - refractive
    glm::vec3 greenColor(0.2f, 0.8f, 0.3f);
    ModelTexture* glossyGreenMaterial = new ModelTexture(greenColor, 0.6f, 0.5f, 32.0f, 0.3f, 1.5f, 1.0f);
    Sphere* sphere2 = new Sphere(glm::vec3(0.0f, 1.5f, 0.0f), 1.5f, 3, glossyGreenMaterial, refractiveShader);

    sphere2->setNeedsEnvMap(true);
    addObject(sphere2);

    // Blue sphere - very reflective
    glm::vec3 blueColor(0.2f, 0.4f, 1.0f);
    ModelTexture* shinyBlueMaterial = new ModelTexture(blueColor, 0.3f, 0.9f, 128.0f, 0.6f);
    Sphere* sphere3 = new Sphere(glm::vec3(4.0f, 1.5f, 0.0f), 1.5f, 3, shinyBlueMaterial, defaultShader);

    sphere3->setNeedsEnvMap(true);
    addObject(sphere3);
}

void Scene::constructObjects(){
    glm::vec3 blueColor(0.2f, 0.4f, 1.0f);
    ModelTexture* shinyBlueMaterial = new ModelTexture(
        blueColor,
        0.3f,
        0.9f,
        128.0f,
        0.5f,
        1.0f,
        1.0f
    );

    planet = new Sphere(
        glm::vec3(4.0f, 1.5f, 0.0f),
        3.0f,
        3,
        shinyBlueMaterial,
        refractiveShader
    );

    planet->setNeedsEnvMap(true);
    addObject(planet);
}

void Scene::constructDebugLights()
{
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

void Scene::init(){
    std::cout << "[Scene] Initializing scene..." << std::endl;

    // Create the default shader
    defaultShader = new ShaderProgram("shaders/template.vert", "shaders/template.frag");
    refractiveShader = new ShaderProgram("shaders/template.vert", "shaders/refractive.frag");

    if (!defaultShader || defaultShader->getProgram() == 0)
    {
        std::cerr << "[Scene] ERROR: Failed to create default shader!" << std::endl;
        return;
    }

    std::cout << "[Scene] Initializing skybox..." << std::endl;
    int numSkybox = rand() % 5;
    skybox = new Skybox("shaders/skybox.vert", "shaders/skybox.frag",
                        "textures/skybox/" + std::to_string(numSkybox) + "/");
    CHECK_GL_ERROR();

    constructObjects();
    CHECK_GL_ERROR();

    constructDebugLights();
    CHECK_GL_ERROR();

    std::cout << "[Scene] Created " << objects.size() << " objects and "
        << lights.size() << " lights" << std::endl;
}

void Scene::cleanup()
{
    // Clean up objects and their associated geometry
    for (Object* obj : objects)
    {
        if (obj)
        {
            // Delete the geometry
            if (obj->getGeometry())
            {
                delete obj->getGeometry();
            }
            // Delete the texture
            delete obj->getTexture();
            delete obj;
        }
    }
    objects.clear();

    for (Light* light : lights)
    {
        delete light;
    }
    lights.clear();

    if (defaultShader)
    {
        delete defaultShader;
        defaultShader = nullptr;
    }

    if (skybox)
    {
        delete skybox;
        skybox = nullptr;
    }
}

void Scene::addObject(Object* obj)
{
    if (obj)
    {
        objects.push_back(obj);
    }
}

void Scene::addLight(Light* light)
{
    if (light)
    {
        lights.push_back(light);
    }
}

void Scene::update(float deltaTime)
{
    static float angle = 0.0f;
    angle += deltaTime * 0.5f;

    for (Object* obj : objects)
        obj->setRotationY(angle * 30.0f);
}
