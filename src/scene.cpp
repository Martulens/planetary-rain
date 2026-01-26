#include "scene.h"
#include "camera.h"
#include "gamestate.h"
#include "config.h"
#include "framework.h"
#include <iostream>
#include <algorithm>

void Scene::updatePlanet(const PlanetParams& params){
    if (!mPlanet) return;

    auto it = std::find(mObjects.begin(), mObjects.end(), mPlanet);
    if (it != mObjects.end()) {
        mObjects.erase(it);
    }

    bool needsEnv = mPlanet->getNeedsEnvMap();
    mPlanet = nullptr;

    auto newMaterial = std::make_shared<ModelTexture>(
        params.color,
        params.pd,
        params.ps,
        params.ns,
        params.reflectivity,
        params.ior,
        params.transparency
    );

    auto shader = mDefaultShader;
    if (params.ior > 1.0f) {
        shader = mRefractiveShader;
    }

    glm::vec3 newPos(params.x, params.y, params.z);
    mPlanet = std::make_shared<Sphere>(  newPos,
                                        params.radius,
                                        params.detail,
                                        params.noise,
                                        params.showTerrain,
                                        newMaterial,
                                        shader);
    mPlanet->setNeedsEnvMap(needsEnv);

    addObject(mPlanet);
}

void Scene::renderSceneForEnvMap(const glm::mat4& view, const glm::mat4& projection, std::shared_ptr<Object> excludeObject){
    if (mSkybox)
        mSkybox->draw(view, projection);

    // Render all mObjects except the excluded one
    for (auto& obj : mObjects)
    {
        if (obj == excludeObject) continue;
        if (!obj || !obj->getGeometry() || !obj->getShader()) continue;

        auto shader = obj->getShader();
        glUseProgram(shader->getProgram());

        glUniformMatrix4fv(shader->getModelMatrixLocation(), 1, GL_FALSE, &obj->getModelMatrix()[0][0]);
        glUniformMatrix4fv(shader->getViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shader->getProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(projection));

        // Simplified uniforms for env map rendering
        auto texture = obj->getTexture();
        glUniform3fv(shader->getBaseColorLocation(), 1, glm::value_ptr(texture->getColor()));
        glUniform1f(shader->getPdLocation(), texture->getPd());
        glUniform1f(shader->getPsLocation(), texture->getPs());
        glUniform1f(shader->getNsLocation(), texture->getNs());
        glUniform3fv(shader->getSkyColorLocation(), 1, glm::value_ptr(SKY_COLOR));

        // Set up mLights
        glUniform1i(glGetUniformLocation(shader->getProgram(), "numLights"), static_cast<int>(mLights.size()));
        for (size_t i = 0; i < mLights.size() && i < LIGHTS_MAX; ++i){
            auto light = mLights[i];
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

void Scene::renderEnvironmentMaps(std::shared_ptr<Camera> camera){
    // Store original viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    for (auto& obj : mObjects){
        if (!obj->getNeedsEnvMap()) continue;

        auto envMap = obj->getEnvMap();
        if (!envMap){
            obj->createEnvMap(128);
            envMap = obj->getEnvMap();
        }

        // Update view matrices from object's position
        envMap->updateViewMatrices(obj->getPosition());

        // Render all 6 faces
        for (int face = 0; face < 6; ++face){
            envMap->beginCapture(face);

            glClearColor(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderSceneForEnvMap(envMap->getViewMatrix(face), envMap->getProjection(), obj);

            envMap->endCapture();
        }
    }

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void Scene::constructDebugObjects(){
    std::cout << "[Scene] Initializing mObjects..." << std::endl;
}

void Scene::constructObjects(){
    PlanetParams defaults;
    auto material = std::make_shared<ModelTexture>(
        defaults.color,
        defaults.pd,
        defaults.ps,
        defaults.ns,
        defaults.reflectivity,
        defaults.ior,
        defaults.transparency
    );

    auto shader = (defaults.ior > 1.0f) ? mRefractiveShader : mDefaultShader;

    mPlanet = std::make_shared<Sphere>(
        glm::vec3(defaults.x, defaults.y, defaults.z),
        defaults.radius,
        defaults.detail,
        defaults.noise,
        defaults.showTerrain,
        material,
        shader
    );

    mPlanet->setNeedsEnvMap(true);
    addObject(mPlanet);
}

void Scene::constructDebugLights()
{
    std::cout << "[Scene] Initializing mLights..." << std::endl;

    auto sunLight = std::make_shared<Light>(
        glm::vec3(1.0f, -1.0f, -0.5f),
        glm::vec3(1.0f, 0.95f, 0.9f),
        1.0f,
        false
    );
    addLight(sunLight);

    /*
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
    */
}

void Scene::init(){
    std::cout << "[Scene] Initializing scene..." << std::endl;

    // Create the default shader
    mDefaultShader = std::make_shared<ShaderProgram>("shaders/mPlanet.vert", "shaders/mPlanet.frag");
    mRefractiveShader = std::make_shared<ShaderProgram>("shaders/mPlanet.vert", "shaders/refractive.frag");

    if (!mDefaultShader || mDefaultShader->getProgram() == 0){
        std::cerr << "[Scene] ERROR: Failed to create default shader!" << std::endl;
        return;
    }

    std::cout << "[Scene] Initializing mSkybox..." << std::endl;
    int numSkybox = rand() % 5;
    mSkybox = std::make_shared<Skybox>("shaders/mSkybox.vert", "shaders/mSkybox.frag",
                        "textures/mSkybox/" + std::to_string(numSkybox) + "/");
    CHECK_GL_ERROR();

    constructObjects();
    CHECK_GL_ERROR();

    constructDebugLights();
    CHECK_GL_ERROR();

    std::cout << "[Scene] Created " << mObjects.size() << " mObjects and "
        << mLights.size() << " mLights" << std::endl;
}

void Scene::addObject(std::shared_ptr<Object> obj){
    if (obj)
        mObjects.push_back(obj);

}

void Scene::addLight(std::shared_ptr<Light> light){
    if (light)
        mLights.push_back(light);
}

void Scene::update(float deltaTime){
    static float angle = 0.0f;
    angle += deltaTime * 0.5f;

    for (auto obj : mObjects)
        obj->setRotationY(angle * 30.0f);
}
