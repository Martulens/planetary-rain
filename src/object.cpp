#include "object.h"

Object::Object(glm::vec3 pos, MeshGeometry* mesh, ShaderProgram* s, ModelTexture* t){
    position = pos;
    geometry = mesh;
    shader = s;
    texture = t;

    // Initialize the model matrix based on position
    updateModelMatrix();
}

void Object::updateModelMatrix() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size));
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