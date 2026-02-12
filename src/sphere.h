#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include "noise.h"

class Sphere : public Object{
private:
    Noise mNoise;

    float mRadius = 1.0f;
    float mOceanLevel = 0.97f;
    int mDetail = 2;
    int mRepeat = 0;
    bool mShowTerrain = false;

    bool mAutoRotate = true;
    float mRotationSpeed = 30.0f;
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
    bool mWavesEnabled = false;
    int mNumWaves = 0;
    std::vector<WaveSettings> mWaves;

    void setWavesEnabled(bool e) { mWavesEnabled = e; }
    void setNumWaves(int n) { mNumWaves = n; }
    void setWaves(const std::vector<WaveSettings>& w) { mWaves = w; }
    void setOceanLevel(float level){ mOceanLevel = level; };

    float getOceanLevel() const override {  return mOceanLevel; };
    bool getWavesEnabled() const override { return mWavesEnabled; }
    int getNumWaves() const override { return mNumWaves; }
    const std::vector<WaveSettings>& getWaves() const override { return mWaves; }

};

#endif // SPHERE_H
