#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include "noise.h"

class Sphere : public Object{
private:
    Noise mNoise;

    float mRadius = 1.0f;
    int mDetail = 2;
    int mRepeat = 0;
    bool mShowTerrain = false;

public:
    Sphere() = default;
    Sphere(glm::vec3 position,
           float radius,
           int detail,
           const std::vector<NoiseSettings>& sets,
           bool show,
           std::shared_ptr<ModelTexture> texture,
           std::shared_ptr<ShaderProgram> shader);

    // TODO remake
    glm::vec3 mapColor(float height);
    glm::vec3 mapTerrain(float height);

    /// === MESH GENERATORS
    std::shared_ptr<MeshGeometry> uvSphere();
    std::shared_ptr<MeshGeometry> cubeSphere();

    // === GETTERS
    float getRadius() const override { return mRadius; }
    int getDetail() const { return mDetail; }
    bool getUsingTerrain() const override { return mShowTerrain; }
    Noise getNoise() const override { return mNoise; };

};

#endif // SPHERE_H
