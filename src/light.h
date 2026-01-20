
#ifndef LIGHT_H
#define LIGHT_H

#include "object.h"
#include "shaderProgram.h"
#include "config.h"
#include "glm/vec3.hpp"

class Light : public Object {
private:
    glm::vec3 color;
    float brightness = 0.0f;

    bool pointLight = false;
public:
    Light() = default;
    Light(MeshGeometry* mesh, ShaderProgram* shader, ModelTexture* tex, glm::vec3 color);

    // === GETERS
    glm::vec3 getColor() const { return color; }
    float getBrightness() const { return brightness; }
    bool isPointLight() const { return pointLight; }

};




#endif //LIGHT_H
