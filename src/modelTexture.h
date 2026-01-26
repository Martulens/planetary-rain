#pragma once

#ifndef MODELTEXTURE_H
#define MODELTEXTURE_H

#include <string>
#include <GL/glew.h>

#include "glm/vec3.hpp"

class ModelTexture {
private:
    GLuint mTextureID = 0;
    glm::vec3 mColor = glm::vec3(1.0f);

    int mWidth = 0;
    int mHeight = 0;

    float mPd = 1.0f;
    float mPs = 1.0f;
    float mNs = 1.0f;

    float mReflectivity = 0.0f;

    float mIor = -1.0f;
    bool mRefractive = false;
    float mTransparency = 0.0f;

public:
    ModelTexture() = default;
    ModelTexture(const std::string& fileName);
    ModelTexture(const glm::vec3& color, float pd, float ps, float k, float reflectivity);
    ModelTexture(const glm::vec3& color, float pd, float ps, float k,
        float reflectivity, float ior, float transparency);

    // === GETTERS
    glm::vec3 getColor() const{ return mColor; }
    GLuint getTextureID() const { return mTextureID; }

    // -> light model
    float getPd() const{ return mPd; }
    float getPs() const{ return mPs; }
    float getNs() const{ return mNs; }
    float getReflectivity() const{ return mReflectivity; }
    bool isRefractive() const{ return mRefractive; }
    float getIOR() const{ return mIor; }
    float getTransparency() const{ return mTransparency; }

};


#endif // MODELTEXTURE_H