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
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLuint vao = 0;

    float maxY = 0.0f;
    unsigned int numVertices = 0;
    unsigned int numTriangles = 0;

    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;

    const aiScene* scene = nullptr;
    Assimp::Importer importer;

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

    void setVertex(Vertex vertex);
    void setIndice(std::vector<unsigned int> indices);

    // === GETTERS
    GLuint getVBO() const { return vbo; }
    GLuint getEBO() const { return ebo; }
    GLuint getVAO() const { return vao; }

    unsigned int getNumTriangles() const { return numTriangles; }
    unsigned int getNumVertices() const { return numVertices; }
    float getMaxY() const { return maxY; }
};

#endif // MESH_H