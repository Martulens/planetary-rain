#include "draw.h"
#include "framework.h"
#include "gamestate.h"
#include "context.h"
#include "camera.h"
#include "light.h"
#include "object.h"
#include <vector>

void setupUniforms(const Object* obj, const std::vector<Light*> lights, const Camera* cam, const GameState& gameState) {
    // === VIEW SETUP
    ShaderProgram* shader = obj->getShader();

    glUniformMatrix4fv(shader->getProjectionMatrixLocation(), 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(shader->getViewMatrixLocation(), 1, GL_FALSE, &cam->getViewMatrix()[0][0]);

    glUniform1f(shader->getDensityLocation(), FOG_DENSITY);
    glUniform1f(shader->getGradientLocation(), FOG_GRADIENT);
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
    GLint programLocation = shader->getProgram();

    glUniform1i(glGetUniformLocation(programLocation, "numLights"), static_cast<int>(lightsLen));

    for (int i = 0; i < lightsLen; ++i) {
        if(i >= LIGHTS_MAX)
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
        glUniform1f(glGetUniformLocation(programLocation, uniformName.c_str()), light->isPointLight());
    }

    CHECK_GL_ERROR();


    float currentTime = 0.001f * glutGet(GLUT_ELAPSED_TIME);
    GLint timeLoc = glGetUniformLocation(programLocation, "elapsedTime");
    glUniform1f(timeLoc, currentTime);
}

void setupTextures(const Object* obj) {
    ShaderProgram* shader = obj->getShader();

    glUniform1i(glGetUniformLocation(shader->getProgram(), "modelTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj->getTexture()->getTextureID());

    CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
}



// === draw template
void drawTemplate(const Object* obj, const Camera* cam, const std::vector<Light*>& lights, const GameState& gameState) {
    ShaderProgram* shader = obj->getShader();

    glUseProgram(shader->getProgram());

    setupUniforms(obj, lights, cam, gameState);
    setupTextures(obj);

    if (obj->getGeometry()->getVAO() != 0)
        obj->draw();
    else
        std::cout << "[ERROR] Object geometry not initialized!" << std::endl;

    glUseProgram(0);
}

void drawScene(){
    drawGrid();

    glColor3f(1.0f, 0.0f, 0.0f);

}

void drawWindow() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    // === PROJECTION FROM CAMERA
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(con::camera->getProjectionMatrix()));

    // === VIEW FROM CAMERA
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(con::camera->getViewMatrix()));

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // === DRAW SCENE ===
    drawScene();

    CHECK_GL_ERROR();
}