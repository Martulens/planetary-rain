#pragma once

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <GL/glew.h>

class ShaderProgram {
private:
    GLuint mProgram;
    GLint mPosLocation;
    GLint mColorLocation;

    // Vertex shader uniform locations
    GLint mModelMatrix;
    GLint mProjectionMatrix;
    GLint mViewMatrix;

    GLint mDensity;
    GLint mGradient;

    // Fragment shader uniform locations
    GLint mBaseColor;
    GLint mPs;
    GLint mPd;
    GLint mNs;
    GLint mReflectivity;
    GLint mSkyColor;
    GLint mIor;
    GLint mTransparency;

    // Texture sampler uniforms
    GLint mBackTexture;
    GLint mRTexture;
    GLint mBTexture;
    GLint mGTexture;

    // Terrain
    GLint mUsingTerrain;

    GLint screenCoordLocation;

public:
    ShaderProgram() = default;
    ShaderProgram(const std::string& vert, const std::string& frag);
    ~ShaderProgram();

    void loadFloat(GLint location, float value);
    void init(const std::string& vert, const std::string& frag);

    void getSamplerUniforms();
    void getAllUniforms();
    void bindAttributes();

    // === GETTERS
    // -> shader
    GLuint getProgram() const{ return mProgram; }

    // -> model
    GLint getPosLocation() const { return mPosLocation; }
    GLint getModelMatrixLocation() const { return mModelMatrix; }
    GLint getProjectionMatrixLocation() const { return mProjectionMatrix; }
    GLint getViewMatrixLocation() const { return mViewMatrix; }

    // -> advanced terrain
    GLint getUsingTerrain() const { return mUsingTerrain; };

    // -> material
    GLint getColorLocation() const { return mColorLocation; }
    GLint getBaseColorLocation() const { return mBaseColor; }
    GLint getPsLocation() const { return mPs; }
    GLint getPdLocation() const { return mPd; }
    GLint getNsLocation() const { return mNs; }
    GLint getReflectivityLocation() const { return mReflectivity; }
    GLint getIorLocation() const { return mIor; }
    GLint getTransparencyLocation() const { return mTransparency; }

    // -> environment
    GLint getSkyColorLocation() const { return mSkyColor; }
    GLint getDensityLocation() const { return mDensity; }
    GLint getGradientLocation() const { return mGradient; }

    // -> HDR texture
    GLint getBackTextureLocation() const { return mBackTexture; }
    GLint getRTextureLocation() const { return mRTexture; }
    GLint getBTextureLocation() const { return mBTexture; }
    GLint getGTextureLocation() const { return mGTexture; }
};


#endif //SHADERPROGRAM_H
