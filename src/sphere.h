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

    bool mAutoRotate = false;
    float mRotationSpeed = 10.0f;

    bool mWavesEnabled = false;
    float mOceanLevel = 1.5f;
    float mWaveHeight = 0.005f;
    float mWaveLength = 0.5f;
    float mWaveSpeed = 1.0f;
    float mWaveOffset = 0.0f;

public:
    Sphere() = default;
    Sphere(glm::vec3 position,
           float radius,
           int detail,
           const std::vector<NoiseSettings>& sets,
           bool show,
           std::shared_ptr<ModelTexture> texture,
           std::shared_ptr<ShaderProgram> shader);

    glm::vec3 mapColor(float height);
    glm::vec3 mapTerrain(float height);


    // === ROTATION
    void setAutoRotate(bool on) { mAutoRotate = on; }
    void setRotationSpeed(float speed) { mRotationSpeed = speed; }
    bool getAutoRotate() const { return mAutoRotate; }
    float getRotationSpeed() const { return mRotationSpeed; }

    /// === MESH GENERATORS
    std::shared_ptr<MeshGeometry> uvSphere();
    std::shared_ptr<MeshGeometry> cubeSphere();

    // === UPDATES
    void updateMaterial(const glm::vec3& color, float pd, float ps, float ns,
                                float reflectivity, float ior, float transparency,
                                std::shared_ptr<ShaderProgram> defaultShader,
                                std::shared_ptr<ShaderProgram> refractiveShader);
    void updateNoise(const std::vector<NoiseSettings>& settings, bool showTerrain);
    void updateRadius(float radius);
    void rebuildMesh(int detail);

    // === GETTERS & SETTERS
    float getRadius() const override { return mRadius; }
    int getDetail() const override { return mDetail; }
    bool getUsingTerrain() const override { return mShowTerrain; }
    Noise getNoise() const override { return mNoise; };

    // -> waves
    void setOceanLevel(float l) override{ mOceanLevel = l;}
    void setWavesEnabled(bool e) { mWavesEnabled = e;}
    void setWaveHeight(float v) { mWaveHeight = v; }
    void setWaveLength(float v) { mWaveLength = v; }
    void setWaveSpeed(float v) { mWaveSpeed = v; }
    void setWaveOffset(float v) { mWaveOffset = v; }

    virtual bool getWavesEnabled() const { return mWavesEnabled;}
    float getWaveHeight() const override { return mWaveHeight; }
    float getWaveLength() const override { return mWaveLength; }
    float getWaveSpeed() const override { return mWaveSpeed; }
    float getWaveOffset() const override { return mWaveOffset; }
    float getOceanLevel() const override {  return mOceanLevel; };
};

#endif // SPHERE_H
