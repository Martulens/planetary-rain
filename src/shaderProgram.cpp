#include "shaderProgram.h"
#include <iostream>
#include <vector>
#include "framework.h"

ShaderProgram::ShaderProgram(const std::string& vert, const std::string& frag){
    init(vert, frag);
}

void ShaderProgram::loadFloat(GLint location, float value){
    glUniform1f(location, value);
}

void ShaderProgram::init(const std::string& vert, const std::string& frag){
    std::vector<GLuint> shaderList;
    shaderList.push_back(framework::createShaderFromFile(GL_VERTEX_SHADER, vert));
    shaderList.push_back(framework::createShaderFromFile(GL_FRAGMENT_SHADER, frag));

    std::cout << vert << " " << frag << std::endl;
    mProgram = framework::createProgram(shaderList);

    getAllUniforms();
    getSamplerUniforms();
    bindAttributes();
}

void ShaderProgram::getAllUniforms(){
    // === MVP
    mModelMatrix = glGetUniformLocation(mProgram, "modelMatrix");
    mProjectionMatrix = glGetUniformLocation(mProgram, "projectionMatrix");
    mViewMatrix = glGetUniformLocation(mProgram, "viewMatrix");

    // === FOG
    mDensity = glGetUniformLocation(mProgram, "density");
    mGradient = glGetUniformLocation(mProgram, "gradient");

    // === MATERIAL
    mBaseColor = glGetUniformLocation(mProgram, "baseColor");
    mPd = glGetUniformLocation(mProgram, "pd");
    mPs = glGetUniformLocation(mProgram, "ps");
    mNs = glGetUniformLocation(mProgram, "ns");
    mReflectivity = glGetUniformLocation(mProgram, "reflectivity");
    mIor = glGetUniformLocation(mProgram, "ior");
    mTransparency = glGetUniformLocation(mProgram, "transparency");

    // === AMBIENT
    mSkyColor = glGetUniformLocation(mProgram, "skyColor");

    // === TERRAIN
    mUsingTerrain = glGetUniformLocation(mProgram, "usingTerrain");
}

void ShaderProgram::getSamplerUniforms(){
    mBackTexture = glGetUniformLocation(mProgram, "backTexture");
    mRTexture = glGetUniformLocation(mProgram, "rTexture");
    mGTexture = glGetUniformLocation(mProgram, "gTexture");
    mBTexture = glGetUniformLocation(mProgram, "bTexture");
}

void ShaderProgram::bindAttributes(){
    glBindAttribLocation(mProgram, 0, "position");
    glBindAttribLocation(mProgram, 1, "texCoord");
    glBindAttribLocation(mProgram, 2, "normal");
    glBindAttribLocation(mProgram, 3, "color");
}


ShaderProgram::~ShaderProgram(){
    framework::deleteProgramAndShaders(mProgram);
}
