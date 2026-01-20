#include "skybox.h"
#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

Skybox::Skybox(const std::string& vertShader, const std::string& fragShader,
               const std::string& texturePrefix) {

    std::vector<std::string> faces = {
        texturePrefix + "right.png",
        texturePrefix + "left.png",
        texturePrefix + "top.png",
        texturePrefix + "bottom.png",
        texturePrefix + "front.png",
        texturePrefix + "back.png"
    };

    shader = new ShaderProgram(vertShader, fragShader);

    if (!shader || shader->getProgram() == 0) {
        std::cerr << "[Skybox] Failed to create shader program!" << std::endl;
        return;
    }

    viewMatrixLocation = glGetUniformLocation(shader->getProgram(), "viewMatrix");
    projectionMatrixLocation = glGetUniformLocation(shader->getProgram(), "projectionMatrix");
    skyboxSamplerLocation = glGetUniformLocation(shader->getProgram(), "skyboxSampler");

    static const float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    if (!loadCubemap(faces)) {
        std::cerr << "[Skybox] Failed to load cubemap textures!" << std::endl;
    }

    std::cout << "[Skybox] Initialized successfully" << std::endl;
}

Skybox::~Skybox(){
    cleanup();
}

bool Skybox::loadCubemap(const std::vector<std::string>& facesPaths){
    if (facesPaths.size() != 6)
    {
        std::cerr << "[Skybox] Expected 6 face paths, got " << facesPaths.size() << std::endl;
        return false;
    }

    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    // Don't flip skybox images (they're usually authored correctly)
    stbi_set_flip_vertically_on_load(false);

    GLenum targets[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, // right
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // left
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // top
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // bottom
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // front
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z // back
    };

    for (int i = 0; i < 6; ++i){
        int width, height, channels;
        unsigned char* data = stbi_load(facesPaths[i].c_str(), &width, &height, &channels, 0);

        if (data){
            GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(targets[i], 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            std::cout << "[Skybox] Loaded: " << facesPaths[i] << " (" << width << "x" << height << ")" << std::endl;
            stbi_image_free(data);
        }
        else{
            std::cerr << "[Skybox] Failed to load: " << facesPaths[i] << std::endl;
            std::cerr << "[Skybox] Reason: " << stbi_failure_reason() << std::endl;
            stbi_image_free(data);
            return false;
        }
    }

    // Restore flip setting for other textures
    stbi_set_flip_vertically_on_load(true);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    CHECK_GL_ERROR();
    return true;
}

void Skybox::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix){
    if (!shader || vao == 0 || cubemapTexture == 0){
        return;
    }

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glUseProgram(shader->getProgram());

    // Remove translation from view matrix -> skybox should always be centered on camera
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(viewMatrix));

    // Set uniforms
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewNoTranslation));
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Bind cubemap texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(skyboxSamplerLocation, 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glUseProgram(0);
}

void Skybox::cleanup(){
    if (vao != 0){
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if (vbo != 0){
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if (cubemapTexture != 0){
        glDeleteTextures(1, &cubemapTexture);
        cubemapTexture = 0;
    }

    if (shader != nullptr){
        delete shader;
        shader = nullptr;
    }
}
