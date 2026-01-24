#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include "noise.h"

class Sphere : public Object {
private:
  Noise noise;

  float radius = 1.0f;
  int detail = 2;
  int repeat = 0;

public:
  Sphere() = default;
  Sphere(glm::vec3 position, float radius, int detail, const std::vector<NoiseSettings>& sets, ModelTexture* texture, ShaderProgram* shader);

  MeshGeometry* uvSphere(float radius, int detail);
  MeshGeometry* cubeSphere(float radius, int detail);

  float getRadius() const { return radius; }
  int getDetail() const { return detail; }
};

#endif // SPHERE_H