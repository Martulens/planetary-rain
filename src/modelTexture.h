
#ifndef MODELTEXTURE_H
#define MODELTEXTURE_H

#include <string>
#include <GL/glew.h>

#include "glm/vec3.hpp"

class ModelTexture {
private:
    GLuint textureID = 0;
    glm::vec3 color = glm::vec3(1.0f);

    int width = 0;
    int height = 0;

    float pd = 1.0f;
    float ps = 1.0f;
    float ns = 1.0f;

    float reflectivity = 0.0f;

    float ior = -1.0f;
    bool refractive = false;
    float transparency = 0.0f;

public:
    ModelTexture() = default;
    ModelTexture(const std::string& fileName);
    ModelTexture(const glm::vec3& color, float pd, float ps, float k, float reflectivity);
    ModelTexture(const glm::vec3& color, float pd, float ps, float k,
        float reflectivity, float ior, float transparency);

    // === GETTERS
    glm::vec3 getColor() const{ return color; }
    GLuint getTextureID() const { return textureID; }

    float getPd() const{ return pd; }
    float getPs() const{ return ps; }
    float getNs() const{ return ns; }
    float getReflectivity() const{ return reflectivity; }
    bool isRefractive() const{ return refractive; }
    float getIOR() const{ return ior; }
    float getTransparency() const{ return transparency; }

};


#endif // MODELTEXTURE_H