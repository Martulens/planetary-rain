
#ifndef LIGHT_H
#define LIGHT_H

#include "object.h"
#include "shaderProgram.h"
#include "config.h"
#include "glm/vec3.hpp"

class Light{
private:
    glm::vec3 mPosition;
    glm::vec3 mColor;
    float mBrightness = 0.0f;

    bool mPointLight = false;
public:
    Light() = default;
    Light(  glm::vec3 position,
            glm::vec3 color,
            float brightness,
            bool isPoint)
    : mPosition(position), mColor(color), mBrightness(brightness), mPointLight(isPoint) {};

    // === GETERS
    glm::vec3 getColor() const { return mColor; }
    glm::vec3 getPosition() const { return mPosition; }

    float getBrightness() const { return mBrightness; }
    bool isPointLight() const { return mPointLight; }

};




#endif //LIGHT_H
