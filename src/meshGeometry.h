#pragma once

#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 color;
};

class MeshGeometry {
private:
    GLuint mVbo = 0;
    GLuint mEbo = 0;
    GLuint mVao = 0;

    float mMaxY = 0.0f;
    unsigned int mNumVertices = 0;
    unsigned int mNumTriangles = 0;

    std::vector<unsigned int> mIndices;
    std::vector<Vertex> mVertices;

    const aiScene* mScene = nullptr;
    Assimp::Importer mImporter;

public:
    MeshGeometry() = default;
    MeshGeometry(std::string& fileName);
    MeshGeometry(const char* fileName);
    MeshGeometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    ~MeshGeometry();

    void processNodeHierarchy(aiNode* node, const glm::mat4& mat);

    void loadModel(std::string& fileName);
    void loadVAO();

    void cleanupGeometry();

    // === GETTERS
    GLuint getVBO() const { return mVbo; }
    GLuint getEBO() const { return mEbo; }
    GLuint getVAO() const { return mVao; }

    unsigned int getNumTriangles() const { return mNumTriangles; }
    unsigned int getNumVertices() const { return mNumVertices; }
    float getMaxY() const { return mMaxY; }
};

#endif // MESH_H