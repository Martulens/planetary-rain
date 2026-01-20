#ifndef OBJECT_H
#define OBJECT_H
#include <string>

#include "meshGeometry.h"
#include "modelTexture.h"
#include "shaderProgram.h"
#include "glm/vec3.hpp"


class Object {
private:
    glm::vec3 position;           ///< Object world position.
    glm::vec3 direction;          ///< Facing direction (optional use).
    float rotationY = 0.0f;       ///< Y-axis rotation.
    glm::mat4 modelMatrix;        ///< Model transformation matrix.

    float speed = 0.1f;           ///< Optional speed for movement.
    float size;                   ///< Optional size scalar.

    float starTime;               ///< Custom timer start.
    float currTime;               ///< Current time value.

    ShaderProgram* shader;        ///< Pointer to shader used for this object.
    MeshGeometry* geometry;       ///< Pointer to mesh geometry.
    ModelTexture* texture;        ///< Pointer to object's texture(s).

public:
    Object() = default;
    Object(glm::vec3 position, MeshGeometry* mesh, ShaderProgram* shader, ModelTexture* texture);

    void draw() const;

    // === getters
    ShaderProgram* getShader() const{ return shader; }
    MeshGeometry* getGeometry() const { return geometry; }
    ModelTexture* getTexture() const { return texture; }

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getDirection() const { return direction; }

    float getRotationY() const { return rotationY; }
    float getSpeed() const { return speed; }
    float getSize() const { return size; }

    float getStarTime() const { return starTime; }
    float getCurrTime() const { return currTime; }

};

#endif //OBJECT_H
