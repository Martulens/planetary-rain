#include "modelTexture.h"
#include "framework.h"
#include <iostream>

ModelTexture::ModelTexture(const std::string& fileName) {
    textureID = framework::createTexture(fileName);

    if (textureID == 0) {
        std::cerr << "Failed to load texture: " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glBindTexture(GL_TEXTURE_2D, 0);
}

ModelTexture::ModelTexture(const glm::vec3& color, float pd, float ps, float k, float reflectivity){
    this->textureID = 0;
    this->color = color;
    this->pd = pd;
    this->ps = ps;
    this->ns = k;
    this->reflectivity = reflectivity;
}