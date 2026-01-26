#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>

#include "envMap.h"
#include "meshGeometry.h"
#include "modelTexture.h"
#include "shaderProgram.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Object {
protected:
    glm::vec3 position = glm::vec3(0.0f);    ///< Object world position.
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);  ///< Facing direction.
    float rotationY = 0.0f;                   ///< Y-axis rotation.
    glm::mat4 modelMatrix = glm::mat4(1.0f);  ///< Model transformation matrix.

    float speed = 0.1f;           ///< Optional speed for movement.
    float size = 1.0f;            ///< Optional size scalar.

    float starTime = 0.0f;        ///< Custom timer start.
    float currTime = 0.0f;        ///< Current time value.

    std::shared_ptr<ShaderProgram> shader = nullptr;   ///< Pointer to shader used for this object.
    std::shared_ptr<MeshGeometry> geometry = nullptr;  ///< Pointer to mesh geometry.
    std::shared_ptr<ModelTexture> texture = nullptr;   ///< Pointer to object's texture(s).
    std::shared_ptr<EnvMap> envMap = nullptr;
    bool needsEnvMap = false;

public:
    Object() = default;
    Object( glm::vec3 position,
            std::shared_ptr<MeshGeometry> mesh,
            std::shared_ptr<ShaderProgram> shader,
            std::shared_ptr<ModelTexture> texture);

    virtual ~Object() = default;

    virtual void draw() const;
    void updateModelMatrix();
    void createEnvMap(int resolution = 128);
    void deleteEnvMap();

    // === Setters
    void setPosition(const glm::vec3& pos) { position = pos; updateModelMatrix(); }
    void setRotationY(float rot) { rotationY = rot; updateModelMatrix(); }
    void setSize(float s) { size = s; updateModelMatrix(); }
    void setNeedsEnvMap(bool needs) { needsEnvMap = needs; }

    // === Getters
    std::shared_ptr<ShaderProgram> getShader() const { return shader; }
    std::shared_ptr<MeshGeometry> getGeometry() const { return geometry; }
    std::shared_ptr<ModelTexture> getTexture() const { return texture; }
    std::shared_ptr<EnvMap> getEnvMap() const { return envMap; }
    bool getNeedsEnvMap() const { return needsEnvMap; }
    const glm::mat4& getModelMatrix() const { return modelMatrix; }

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getDirection() const { return direction; }

    float getRotationY() const { return rotationY; }
    float getSpeed() const { return speed; }
    float getSize() const { return size; }

    float getStarTime() const { return starTime; }
    float getCurrTime() const { return currTime; }

    virtual bool getUsingTerrain() const { return false; };
};

#endif // OBJECT_H