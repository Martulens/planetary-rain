#include "light.h"

// \todo Review Id move this to header. Unless there is a plan to have more code here in near future
Light::Light(glm::vec3 position, glm::vec3 color, float brightness, bool isPoint):
    position(position), color(color), brightness(brightness), pointLight(isPoint) {};
