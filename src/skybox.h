#ifndef SKYBOX_H
#define SKYBOX_H

#include "framework.h"
#include "shaderProgram.h"
#include "modelTexture.h"
#include <string>
#include <vector>


class Skybox {
private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint cubemapTexture = 0;
    ShaderProgram* shader = nullptr;

    GLint viewMatrixLocation = -1;
    GLint projectionMatrixLocation = -1;
    GLint skyboxSamplerLocation = -1;

public:
    Skybox() = default;
    Skybox(const std::string& vertShader, const std::string& fragShader,
           const std::vector<std::string>& facesPaths);

    Skybox(const std::string& vertShader, const std::string& fragShader,
           const std::string& texturePrefix = "textures/skybox/");
    ~Skybox();

    bool loadCubemap(const std::vector<std::string>& facesPaths);
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    void cleanup();

    // Getters
    GLuint getCubemapTexture() const { return cubemapTexture; }
    ShaderProgram* getShader() const { return shader; }
};

#endif // SKYBOX_H