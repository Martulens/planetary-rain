#include "envMap.h"
#include <iostream>

#include "modelTexture.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

EnvMap::EnvMap(int res) {
    init(res);
}

EnvMap::~EnvMap() {
    cleanup();
}

void EnvMap::init(int res) {
    mResolution = res;

    // Create mCubemap texture
    glGenTextures(1, &mCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemap);

    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     mResolution, mResolution, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Create mFramebuffer
    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

    // Create depth renderbuffer
    glGenRenderbuffers(1, &mDepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mResolution, mResolution);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Setup projection matrix
    // -> 90 degree FOV for mCubemap faces
    mCaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

    std::cout << "[EnvMap] Initialized with mResolution " << mResolution << std::endl;
}

void EnvMap::updateViewMatrices(const glm::vec3& position) {
    // View matrices for each mCubemap face
    mCaptureViews[0] = glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));  // +X
    mCaptureViews[1] = glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // -X
    mCaptureViews[2] = glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));   // +Y
    mCaptureViews[3] = glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)); // -Y
    mCaptureViews[4] = glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));  // +Z
    mCaptureViews[5] = glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // -Z
}

void EnvMap::beginCapture(int faceIndex) {
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, mCubemap, 0);
    glViewport(0, 0, mResolution, mResolution);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EnvMap::endCapture() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EnvMap::cleanup() {
    if (mFramebuffer != 0) {
        glDeleteFramebuffers(1, &mFramebuffer);
        mFramebuffer = 0;
    }
    if (mCubemap != 0) {
        glDeleteTextures(1, &mCubemap);
        mCubemap = 0;
    }
    if (mDepthRenderBuffer != 0) {
        glDeleteRenderbuffers(1, &mDepthRenderBuffer);
        mDepthRenderBuffer = 0;
    }
}