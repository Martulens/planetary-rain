#include "draw.h"

#include <memory>

#include "framework.h"
#include "gamestate.h"
#include "camera.h"
#include "light.h"
#include "object.h"
#include "scene.h"
#include "skybox.h"
#include "debug.h"
#include <vector>


void Draw::setupLights(std::shared_ptr<Scene> scene, std::shared_ptr<ShaderProgram> shader){
    // === LIGHTS SETUP
    GLint programLocation = shader->getProgram();

    std::vector<std::shared_ptr<Light>> lights = scene->getLights();
    size_t lightsLen = lights.size();

    glUniform1i(glGetUniformLocation(programLocation, "numLights"), static_cast<int>(lightsLen));

    for (size_t i = 0; i < lightsLen; ++i){
        if (i >= LIGHTS_MAX)
            break;

        std::string uniformName = "lightPositions[" + std::to_string(i) + "]";

        std::shared_ptr<Light> light = lights[i];
        glm::vec3 pos = light->getPosition();
        glUniform3f(glGetUniformLocation(programLocation, uniformName.c_str()), pos.x, pos.y, pos.z);

        uniformName = "lightColors[" + std::to_string(i) + "]";
        glm::vec3 col = light->getColor();
        glUniform3f(glGetUniformLocation(programLocation, uniformName.c_str()), col.r, col.g, col.b);

        uniformName = "lightBrightness[" + std::to_string(i) + "]";
        glUniform1f(glGetUniformLocation(programLocation, uniformName.c_str()), light->getBrightness());

        uniformName = "isPointLight[" + std::to_string(i) + "]";
        glUniform1i(glGetUniformLocation(programLocation, uniformName.c_str()), light->isPointLight() ? 1 : 0);
    }

    CHECK_GL_ERROR();
}

void Draw::setupTime(GLint programLocation){
    float currentTime = 0.001f * glutGet(GLUT_ELAPSED_TIME);
    GLint timeLoc = glGetUniformLocation(programLocation, "time");
    glUniform1f(timeLoc, currentTime);
}

void Draw::setupMaterial(std::shared_ptr<ShaderProgram> shader, std::shared_ptr<ModelTexture> texture){
    // === MATERIAL SETUP
    glUniform1f(shader->getPdLocation(), texture->getPd());
    glUniform1f(shader->getPsLocation(), texture->getPs());
    glUniform1f(shader->getNsLocation(), texture->getNs());
    glUniform1f(shader->getReflectivityLocation(), texture->getReflectivity());

    if (texture->isRefractive()){
        glUniform1f(shader->getIorLocation(), texture->getIOR());
        glUniform1f(shader->getTransparencyLocation(), texture->getTransparency());
    }

    glm::vec3 baseCol = texture->getColor();
    glUniform3f(shader->getBaseColorLocation(),
                baseCol.r, baseCol.g, baseCol.b);

    glUniform3f(shader->getSkyColorLocation(),
                SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b);
    CHECK_GL_ERROR();
}

void Draw::setupObject(std::shared_ptr<Object> object){
    auto shader = object->getShader();
    GLint programLocation = shader->getProgram();

    glUniform1i(shader->getUsingTerrain(), object->getUsingTerrain());

    auto noises = object->getNoise().getSettings();
    size_t numNoises = noises.size();

    glUniform1i(glGetUniformLocation(programLocation, "numNoises"), static_cast<int>(numNoises));
    glUniform1f(glGetUniformLocation(programLocation, "radius"), object->getRadius());

    for (size_t i = 0; i < numNoises; ++i){
        if (i >= NOISE_MAX)
            break;

        NoiseSettings settings = noises[i];

        std::string uniformName = "shown[" + std::to_string(i) + "]";
        glUniform1i(glGetUniformLocation(programLocation, uniformName.c_str()),
                    settings.shown);

        uniformName = "octaves[" + std::to_string(i) + "]";
        glUniform1i(glGetUniformLocation(programLocation, uniformName.c_str()),
                    settings.octaves);

        uniformName = "frequency[" + std::to_string(i) + "]";
        glUniform1f(glGetUniformLocation(programLocation, uniformName.c_str()),
                    settings.frequency);

        uniformName = "amplitude[" + std::to_string(i) + "]";
        glUniform1f(glGetUniformLocation(programLocation, uniformName.c_str()),
                    settings.amplitude);

        uniformName = "persistence[" + std::to_string(i) + "]";
        glUniform1f(glGetUniformLocation(programLocation, uniformName.c_str()),
                    settings.persistence);

        uniformName = "roughness[" + std::to_string(i) + "]";
        glUniform1f(glGetUniformLocation(programLocation, uniformName.c_str()),
                    settings.roughness);

        uniformName = "noiseOffset[" + std::to_string(i) + "]";
        glUniform1f(glGetUniformLocation(programLocation, uniformName.c_str()),
                    settings.offset);
    }

    CHECK_GL_ERROR();

}

void Draw::setupGeneral(std::shared_ptr<Object> object, std::shared_ptr<Camera> cam){
    // === VIEW SETUP
    std::shared_ptr<ShaderProgram> shader = object->getShader();
    GLint programLocation = shader->getProgram();

    glUniformMatrix4fv( shader->getProjectionMatrixLocation(),
                        1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    glUniformMatrix4fv( shader->getViewMatrixLocation(),
                        1, GL_FALSE, &cam->getViewMatrix()[0][0]);

    glUniform1f(shader->getDensityLocation(), FOG_DENSITY);
    glUniform1f(shader->getGradientLocation(), FOG_GRADIENT);

    glm::vec3 camPos = cam->getPosition();
    GLint camPosLoc = glGetUniformLocation(programLocation, "cameraPosition");
    glUniform3f(camPosLoc, camPos.x, camPos.y, camPos.z);

    CHECK_GL_ERROR();

}


void Draw::setupUniforms(std::shared_ptr<Object> object, std::shared_ptr<Camera> cam, std::shared_ptr<Scene> scene){
    setupGeneral(object, cam);
    setupObject(object);
    setupMaterial(object->getShader(), object->getTexture());
    setupLights(scene, object->getShader());
    setupTime(object->getShader()->getProgram());
}

void Draw::setupTextures(std::shared_ptr<Object> obj, std::shared_ptr<Scene> scene){
    std::shared_ptr<ShaderProgram> shader = obj->getShader();
    GLuint programLocation = shader->getProgram();
    GLuint texID = obj->getTexture()->getTextureID();

    // Diffuse texture on unit 0
    glUniform1i(glGetUniformLocation(programLocation, "textureSampler"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID != 0 ? texID : 0);

    // Skybox cubemap on unit 1
    std::shared_ptr<Skybox> skybox = scene->getSkybox();
    if (skybox && skybox->getCubemapTexture() != 0){
        glUniform1i(glGetUniformLocation(programLocation, "skyboxSampler"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getCubemapTexture());
        glUniform1i(glGetUniformLocation(programLocation, "useEnvironmentLighting"), 1);
    }
    else{
        glUniform1i(glGetUniformLocation(programLocation, "useEnvironmentLighting"), 0);
    }

    // Dynamic environment map on unit 2
    std::shared_ptr<EnvMap> envMap = obj->getEnvMap();
    if (envMap && envMap->getCubemap() != 0){
        glUniform1i(glGetUniformLocation(programLocation, "envMapSampler"), 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envMap->getCubemap());
        glUniform1i(glGetUniformLocation(programLocation, "useDynamicReflections"), 1);
    }
    else
    {
        glUniform1i(glGetUniformLocation(programLocation, "useDynamicReflections"), 0);
    }

    CHECK_GL_ERROR();
}

void Draw::drawTemplate(std::shared_ptr<Object> obj, std::shared_ptr<Camera> camera, std::shared_ptr<Scene> scene){
    auto shader = obj->getShader();

    glUseProgram(shader->getProgram());
    glUniformMatrix4fv( shader->getModelMatrixLocation(),
                        1, GL_FALSE, &obj->getModelMatrix()[0][0]);

    setupUniforms(obj, camera, scene);
    setupTextures(obj, scene);

    if (obj->getGeometry()->getVAO() != 0){
        if (obj->getTexture()->isRefractive()){
            glDisable(GL_CULL_FACE);
            obj->draw();
            glEnable(GL_CULL_FACE);

        }else
            obj->draw();
    }
    else
        std::cout << "[ERROR] Object geometry not initialized!" << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void Draw::drawScene(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, std::shared_ptr<GameState> gameState){
    // Draw debug grid
    // drawGrid();

    // Render all scene objects using drawTemplate
    if (scene && camera && gameState){
        const std::vector<std::shared_ptr<Object>> objects = scene->getObjects();
        const std::vector<std::shared_ptr<Light>> lights = scene->getLights();

        for (auto obj : objects){
            if (obj && obj->getGeometry() && obj->getShader()){
                drawTemplate(obj, camera, scene);
                CHECK_GL_ERROR();
            }
        }

        std::shared_ptr<Skybox> skybox = scene->getSkybox();
        if (skybox){
            skybox->draw(camera->getViewMatrix(), camera->getProjectionMatrix());
            CHECK_GL_ERROR();
        }
    }
}

void Draw::drawWindow(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, std::shared_ptr<GameState> gameState){
    static float lastTime = 0.0f;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float deltaTime = currentTime - lastTime;

    lastTime = currentTime;
    if (deltaTime > 0.1f)
        deltaTime = 0.016f;

    // Update scene
    if(!camera)
        return;

    if (scene){
        scene->update(deltaTime);
        scene->renderEnvironmentMaps(camera);
    }

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);
    glClearColor(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(camera->getProjectionMatrix()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(camera->getViewMatrix()));

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    if(!gameState)
        return;

    if (gameState->wireframeMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw the scene
    if (scene)
        scene->update(deltaTime);

    drawScene(scene, camera, gameState);
}
