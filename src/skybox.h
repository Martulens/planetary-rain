#pragma once

#ifndef SKYBOX_H
#define SKYBOX_H

#include <memory>

#include "framework.h"
#include "shaderProgram.h"
#include "modelTexture.h"
#include <string>
#include <vector>


class Skybox {
private:
    GLuint mVao = 0;
    GLuint mVbo = 0;
    GLuint mCubemapTexture = 0;
    std::shared_ptr<ShaderProgram> shader = nullptr;

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

    // === GETTERS
    GLuint getCubemapTexture() const { return mCubemapTexture; }
    std::shared_ptr<ShaderProgram> getShader() const { return shader; }
};

#endif // SKYBOX_H