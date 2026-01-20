
#ifndef LIGHT_H
#define LIGHT_H

#include "object.h"
#include "shaderProgram.h"
#include "config.h"
#include "glm/vec3.hpp"

class Light{
private:
    glm::vec3 position;
    glm::vec3 color;
    float brightness = 0.0f;

    bool pointLight = false;
public:
    Light() = default;
    Light(glm::vec3 position, glm::vec3 color, float brightness, bool isPoint);

    // === GETERS
    glm::vec3 getColor() const { return color; }
    glm::vec3 getPosition() const { return position; }

    float getBrightness() const { return brightness; }
    bool isPointLight() const { return pointLight; }

};




#endif //LIGHT_H
