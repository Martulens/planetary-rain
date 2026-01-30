#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>

#include "envMap.h"
#include "meshGeometry.h"
#include "modelTexture.h"
#include "noise.h"
#include "shaderProgram.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Object {
protected:
    glm::vec3 mPosition = glm::vec3(0.0f);
    glm::vec3 mDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    float mRotationY = 0.0f;
    glm::mat4 mModelMatrix = glm::mat4(1.0f);

    float mSpeed = 0.1f;
    float mSize = 1.0f;

    float mStarTime = 0.0f;
    float mCurrTime = 0.0f;

    std::shared_ptr<ShaderProgram> mShader = nullptr;
    std::shared_ptr<MeshGeometry> mGeometry = nullptr;
    std::shared_ptr<ModelTexture> mTexture = nullptr;
    std::shared_ptr<EnvMap> mEnvMap = nullptr;
    bool mNeedsEnvMap = false;

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

    // === SETTERS
    void setPosition(const glm::vec3& pos) { mPosition = pos; updateModelMatrix(); }
    void setRotationY(float rot) { mRotationY = rot; updateModelMatrix(); }
    void setSize(float s) { mSize = s; updateModelMatrix(); }
    void setNeedsEnvMap(bool needs) { mNeedsEnvMap = needs; }

    // === GETTERS
    // -> model properties
    const glm::mat4& getModelMatrix() const { return mModelMatrix; }
    glm::vec3 getPosition() const { return mPosition; }
    glm::vec3 getDirection() const { return mDirection; }

    std::shared_ptr<ShaderProgram> getShader() const { return mShader; }
    std::shared_ptr<MeshGeometry> getGeometry() const { return mGeometry; }
    std::shared_ptr<ModelTexture> getTexture() const { return mTexture; }
    std::shared_ptr<EnvMap> getEnvMap() const { return mEnvMap; }
    bool getNeedsEnvMap() const { return mNeedsEnvMap; }

    // -> terrain
    virtual bool getUsingTerrain() const { return false; };
    virtual Noise getNoise() const { return Noise(); };


    // -> interaction
    float getRotationY() const { return mRotationY; }
    float getSpeed() const { return mSpeed; }
    float getSize() const { return mSize; }

    float getStarTime() const { return mStarTime; }
    float getCurrTime() const { return mCurrTime; }
};

#endif // OBJECT_H