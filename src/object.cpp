#include "object.h"

Object::Object(glm::vec3 pos, MeshGeometry* mesh, ShaderProgram* s, ModelTexture* t){
    position = pos;
    geometry = mesh;
    shader = s;
    texture = t;
}

void Object::draw() const{
    if (geometry != nullptr){
        glUseProgram(shader->getProgram());
        glBindVertexArray(geometry->getVAO());

        glDrawElements(GL_TRIANGLES, geometry->getNumTriangles() * 3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
}
