#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"

class Sphere : public Object {
private:
  float radius = 1.0f;
  int detail = 2;

public:
  Sphere() = default;
  Sphere(glm::vec3 position, float radius, int detail, ModelTexture* texture, ShaderProgram* shader);

  MeshGeometry* uvSphere(float radius, int detail);
  MeshGeometry* cubeSphere(float radius, int detail);

  float getRadius() const { return radius; }
  int getDetail() const { return detail; }
};

#endif // SPHERE_H