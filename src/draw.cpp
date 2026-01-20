#include "draw.h"
#include "framework.h"
#include "gamestate.h"
#include "context.h"
#include "camera.h"
#include "light.h"
#include "object.h"
#include "scene.h"
#include "debug.h"
#include <vector>

void setupUniforms(const Object* obj, const std::vector<Light*>& lights, const Camera* cam, const GameState& gameState) {
    // === VIEW SETUP
    ShaderProgram* shader = obj->getShader();
    GLint programLocation = shader->getProgram();

    glUniformMatrix4fv(shader->getProjectionMatrixLocation(), 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(shader->getViewMatrixLocation(), 1, GL_FALSE, &cam->getViewMatrix()[0][0]);

    glUniform1f(shader->getDensityLocation(), FOG_DENSITY);
    glUniform1f(shader->getGradientLocation(), FOG_GRADIENT);

    // Camera position for specular calculations
    glm::vec3 camPos = cam->getPosition();
    GLint camPosLoc = glGetUniformLocation(programLocation, "cameraPosition");
    glUniform3f(camPosLoc, camPos.x, camPos.y, camPos.z);

    CHECK_GL_ERROR();

    // === TEXTURE -- ONLY COLOR -- SETUP
    ModelTexture* texture = obj->getTexture();

    glUniform1f(shader->getPdLocation(), texture->getPd());
    glUniform1f(shader->getPsLocation(), texture->getPs());
    glUniform1f(shader->getNsLocation(), texture->getNs());
    glUniform1f(shader->getReflectivityLocation(), texture->getReflectivity());

    glm::vec3 baseCol = texture->getColor();
    glUniform3f(shader->getBaseColorLocation(), baseCol.r, baseCol.g, baseCol.b);

    glUniform3f(shader->getSkyColorLocation(), skyColorConst.r, skyColorConst.g, skyColorConst.b);
    CHECK_GL_ERROR();

    // === LIGHTS SETUP
    size_t lightsLen = lights.size();

    glUniform1i(glGetUniformLocation(programLocation, "numLights"), static_cast<int>(lightsLen));

    for (size_t i = 0; i < lightsLen; ++i) {
        if (i >= LIGHTS_MAX)
            break;

        std::string uniformName = "lightPositions[" + std::to_string(i) + "]";

        Light* light = lights[i];
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

    float currentTime = 0.001f * glutGet(GLUT_ELAPSED_TIME);
    GLint timeLoc = glGetUniformLocation(programLocation, "time");
    glUniform1f(timeLoc, currentTime);
}

void setupTextures(const Object* obj) {
    ShaderProgram* shader = obj->getShader();

    glUniform1i(glGetUniformLocation(shader->getProgram(), "textureSampler"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj->getTexture()->getTextureID());

    CHECK_GL_ERROR();
}

void drawTemplate(const Object* obj, const Camera* cam, const std::vector<Light*>& lights, const GameState& gameState) {
    ShaderProgram* shader = obj->getShader();

    glUseProgram(shader->getProgram());

    // Set model matrix for this object
    glUniformMatrix4fv(shader->getModelMatrixLocation(), 1, GL_FALSE, &obj->getModelMatrix()[0][0]);

    setupUniforms(obj, lights, cam, gameState);
    setupTextures(obj);

    if (obj->getGeometry()->getVAO() != 0)
        obj->draw();
    else
        std::cout << "[ERROR] Object geometry not initialized!" << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void drawScene() {
    // Draw debug grid
    drawGrid();

    // Render all scene objects using drawTemplate
    if (con::scene && con::camera && con::gameState) {
        const std::vector<Object*>& objects = con::scene->getObjects();
        const std::vector<Light*>& lights = con::scene->getLights();

        for (Object* obj : objects) {
            if (obj && obj->getGeometry() && obj->getShader()) {
                drawTemplate(obj, con::camera, lights, *con::gameState);
            }
        }
    }
}

void drawWindow() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    // For the debug grid (using fixed-function pipeline)
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(con::camera->getProjectionMatrix()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(con::camera->getViewMatrix()));

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw the scene
    drawScene();

    CHECK_GL_ERROR();
}