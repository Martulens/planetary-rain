#ifndef EnvMap_H
#define EnvMap_H

#include "framework.h"
#include <glm/glm.hpp>

#include "config.h"

class EnvMap {
private:
    GLuint mFramebuffer = 0;
    GLuint mCubemap = 0;
    GLuint mDepthRenderBuffer = 0;
    int mResolution = ENV_MAP_RESOLUTION;

    glm::mat4 mCaptureProjection;
    glm::mat4 mCaptureViews[6];

public:
    EnvMap() = default;
    EnvMap(int resolution);
    ~EnvMap();

    void init(int res);
    void cleanup();

    void beginCapture(int faceIndex);
    void endCapture();

    void updateViewMatrices(const glm::vec3& position);

    // === GETTERS
    GLuint getCubemap() const { return mCubemap; }
    GLuint getFramebuffer() const { return mFramebuffer; }
    int getResolution() const { return mResolution; }
    glm::mat4 getProjection() const { return mCaptureProjection; }
    glm::mat4 getViewMatrix(int face) const { return mCaptureViews[face]; }
};

#endif // EnvMap_H