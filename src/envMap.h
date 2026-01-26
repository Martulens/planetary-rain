#ifndef EnvMap_H
#define EnvMap_H

#include "framework.h"
#include <glm/glm.hpp>

class EnvMap {
private:
    GLuint framebuffer = 0;
    GLuint cubemap = 0;
    GLuint depthRenderbuffer = 0;
    int resolution = 256;

    glm::mat4 captureProjection;
    glm::mat4 captureViews[6];

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
    GLuint getCubemap() const { return cubemap; }
    GLuint getFramebuffer() const { return framebuffer; }
    int getResolution() const { return resolution; }
    glm::mat4 getProjection() const { return captureProjection; }
    glm::mat4 getViewMatrix(int face) const { return captureViews[face]; }
};

#endif // EnvMap_H