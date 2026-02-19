#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include "noise.h"

class Sphere : public Object{
private:
    float mRadius = 1.0f;
    int mDetail = 2;
    int mRepeat = 0;
    bool mShowTerrain = false;

    bool mAutoRotate = false;
    float mRotationSpeed = 10.0f;

    float mOceanLevel = 0.97f;
    bool mWavesEnabled = false;
    float mFadeE0 = 0.01f;
    float mFadeE1 = 0.1f;

    std::vector<WaveSettings> mWaves;
    Noise mNoise;

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
    void setOceanLevel(float l) { mOceanLevel = l;}
    void setWavesEnabled(bool e) { mWavesEnabled = e;}
    void setFades(float e0, float e1){ mFadeE0 = e0; mFadeE1 = e1; };


    bool getWavesEnabled() const override { return mWavesEnabled;}
    float getOceanLevel() const override {return mOceanLevel};
    glm::vec2 getFades() const override {return glm::vec2(mFadeE0, mFadeE1);};
    std::vector<WaveSettings> getWaves() const override { return mWaves; }
};

#endif // SPHERE_H
