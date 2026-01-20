#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"


class Sphere : public Object {
private:
  float radius = 1.0f;
  int detail = 2;
public:
  Sphere() = default;
  Sphere(glm::vec3 position, int radius, int detail, ModelTexture* texture, ShaderProgram* shader);

  MeshGeometry* generateGeometry(float radius, int detail);
};



#endif //SPHERE_H
