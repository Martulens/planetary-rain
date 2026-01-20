#include "light.h"

Light::Light(glm::vec3 position, glm::vec3 color, float brightness, bool isPoint):
    position(position), color(color), brightness(brightness), pointLight(isPoint) {};
