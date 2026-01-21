#ifndef SPHERE_H
#define SPHERE_H

#include "noise.h"
#include "object.h"

struct Terrain {
    float baseRoughness = 1.0f;
    float roughness = 2.0f;
    float persistence = 0.5f;

    int octaves = 4;

    float strength = 0.3f;
    float minValue = 0.0f;
    float maxValue = 1.0f;

    glm::vec3 center = glm::vec3(0);
};

class Sphere : public Object{
private:
    float radius = 1.0f;
    int detail = 2;
    Terrain terrain;
public:
    Sphere() = default;
    Sphere(glm::vec3 position, float radius, int detail, ModelTexture* texture, ShaderProgram* shader, Terrain terrain);
    Sphere(glm::vec3 position, float radius, int detail, ModelTexture* texture, ShaderProgram* shader);

    MeshGeometry* UVSphere(float radius, int detail);
    void normalizedCube(float radius, int detail);
    MeshGeometry* cubeSphere(float radius, int detail, int seed);
    float getElevation(PerlinNoise noise, glm::vec3 pointOnSphere);


    // === GETTERS
    float getRadius() const { return radius; }
    int getDetail() const { return detail; }
};

#endif // SPHERE_H
