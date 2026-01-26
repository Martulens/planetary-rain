#include "object.h"

Object::Object(glm::vec3 pos, std::shared_ptr<MeshGeometry> mesh, std::shared_ptr<ShaderProgram> s, std::shared_ptr<ModelTexture> t){
    mPosition = pos;
    mGeometry = mesh;
    mShader = s;
    mTexture = t;

    // Initialize the model matrix based on mPosition
    updateModelMatrix();
}

void Object::updateModelMatrix() {
    mModelMatrix = glm::mat4(1.0f);
    mModelMatrix = glm::translate(mModelMatrix, mPosition);
    mModelMatrix = glm::rotate(mModelMatrix, glm::radians(mRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    mModelMatrix = glm::scale(mModelMatrix, glm::vec3(mSize));
}

void Object::createEnvMap(int resolution) {
    if (!mEnvMap)
        mEnvMap = std::make_shared<EnvMap>(resolution);
}

void Object::deleteEnvMap() {
    if (mEnvMap)
        mEnvMap = nullptr;
}

void Object::draw() const{
    if (mGeometry != nullptr){
        glUseProgram(mShader->getProgram());
        glBindVertexArray(mGeometry->getVAO());

        glDrawElements(GL_TRIANGLES, mGeometry->getNumTriangles() * 3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
}
