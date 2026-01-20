#include "shaderProgram.h"
#include <iostream>
#include <vector>
#include "context.h"
#include "framework.h"

ShaderProgram::ShaderProgram(const std::string& vert, const std::string& frag) {
    init(vert, frag);
}

void ShaderProgram::loadFloat(GLint location, float value) {
    glUniform1f(location, value);
}

void ShaderProgram::init(const std::string& vert, const std::string& frag) {
    std::vector<GLuint> shaderList;
    shaderList.push_back(framework::createShaderFromFile(GL_VERTEX_SHADER, vert));
    shaderList.push_back(framework::createShaderFromFile(GL_FRAGMENT_SHADER, frag));

    std::cout << vert << " " << frag << std::endl;
    program = framework::createProgram(shaderList);

    getAllUniforms();
    getSamplerUniforms();
    bindAttributes();
}

void ShaderProgram::getAllUniforms() {
    // === MVP
    modelMatrix = glGetUniformLocation(program, "modelMatrix");
    projectionMatrix = glGetUniformLocation(program, "projectionMatrix");
    viewMatrix = glGetUniformLocation(program, "viewMatrix");

    // === FOG
    density = glGetUniformLocation(program, "density");
    gradient = glGetUniformLocation(program, "gradient");

    // === MATERIAL
    baseColor = glGetUniformLocation(program, "baseColor");
    pd = glGetUniformLocation(program, "pd");
    ps = glGetUniformLocation(program, "ps");
    ns = glGetUniformLocation(program, "ns");
    reflectivity = glGetUniformLocation(program, "reflectivity");

    // === AMBIENT
    skyColor = glGetUniformLocation(program, "skyColor");
}

void ShaderProgram::getSamplerUniforms() {
    backTexture = glGetUniformLocation(program, "backTexture");
    rTexture = glGetUniformLocation(program, "rTexture");
    gTexture = glGetUniformLocation(program, "gTexture");
    bTexture = glGetUniformLocation(program, "bTexture");
    blendMap = glGetUniformLocation(program, "blendMap");
}

void ShaderProgram::bindAttributes() {
    glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "texCoord");
    glBindAttribLocation(program, 2, "normal");
    glBindAttribLocation(program, 3, "color");
}


ShaderProgram::~ShaderProgram() {
    framework::deleteProgramAndShaders(program);
}