
#ifndef MODELTEXTURE_H
#define MODELTEXTURE_H

#include <string>
#include <GL/glew.h>

#include "glm/vec3.hpp"

class ModelTexture {
private:
    GLuint textureID;
    glm::vec3 color;

    int width = 0;
    int height = 0;

    float pd = 1.0f;
    float ps = 1.0f;
    float ns = 1.0f;

    float reflectivity = 0.0;

public:
    ModelTexture() = default;
    ModelTexture(const std::string& fileName);
    ModelTexture(glm::vec3& color, float pd, float ps, float k, float reflectivity);

    // === GETTERS
    glm::vec3 getColor() const{ return color; }
    GLuint getTextureID() const { return textureID; }

    float getPd() const{ return pd; }
    float getPs() const{ return ps; }
    float getNs() const{ return ns; }
    float getReflectivity() const{ return reflectivity; }

};


#endif // MODELTEXTURE_H