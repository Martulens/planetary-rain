#include "modelTexture.h"
#include "framework.h"
#include <iostream>

ModelTexture::ModelTexture(const std::string& fileName) {
    mTextureID = framework::createTexture(fileName);

    if (mTextureID == 0) {
        std::cerr << "Failed to load texture: " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &mWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &mHeight);
    glBindTexture(GL_TEXTURE_2D, 0);
}

ModelTexture::ModelTexture(const glm::vec3& color, float pd, float ps, float k, float reflectivity, float ior, float transparency){
    this->mTextureID = 0;
    this->mColor = color;
    this->mPd = pd;
    this->mPs = ps;
    this->mNs = k;
    this->mReflectivity = reflectivity;

    if(ior <= 0.0f)
        mRefractive = false;
    else{
        mRefractive = true;
        this->mIor = ior;
        this->mTransparency = transparency;
    }
}

ModelTexture::ModelTexture(const glm::vec3& color, float pd, float ps, float k, float reflectivity){
    this->mTextureID = 0;
    this->mColor = color;
    this->mPd = pd;
    this->mPs = ps;
    this->mNs = k;
    this->mReflectivity = reflectivity;
}