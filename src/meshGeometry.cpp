#include "meshGeometry.h"

MeshGeometry::MeshGeometry(std::string& fileName) {
    loadModel(fileName);
}

MeshGeometry::MeshGeometry(const char* fileName) {
    std::string inputPath(fileName);
    loadModel(inputPath);
}

MeshGeometry::MeshGeometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices): indices(indices), vertices(vertices) {
    numVertices = vertices.size();
    size_t numTriangles = indices.size() / 3;
    size_t numVertices = vertices.size();

    loadVAO();
}


void MeshGeometry::processNodeHierarchy(aiNode* node, const glm::mat4& parentTransform) {
    std::string nodeName(node->mName.C_Str());

    glm::mat4 nodeTransform = glm::transpose(glm::make_mat4(&node->mTransformation.a1));
    glm::mat4 globalTransform = parentTransform * nodeTransform;

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNodeHierarchy(node->mChildren[i], globalTransform);
    }
}

void MeshGeometry::loadModel(std::string& fileName) {
    scene = importer.ReadFile(fileName,
        aiProcess_Triangulate | aiProcess_FlipUVs |
        aiProcess_GenNormals | aiProcess_LimitBoneWeights);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    processNodeHierarchy(scene->mRootNode, glm::mat4(1.0f));

    vertices.clear();
    indices.clear();

    unsigned int vertexOffset = 0;
    std::vector<int> bonesAssigned;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            Vertex vertex;
            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            vertex.texCoord = mesh->HasTextureCoords(0)
                ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f);

            vertex.color = glm::vec3(1.0f);
            maxY = std::max(maxY, mesh->mVertices[i].y);

            vertices.push_back(vertex);
        }

        bonesAssigned.resize(vertices.size(), 0);

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }

        vertexOffset += mesh->mNumVertices;
    }

    numTriangles = indices.size() / 3;

    loadVAO();
}

void MeshGeometry::loadVAO() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    GLsizei stride = sizeof(Vertex);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(2); // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(1); // texCoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(3); // color
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, color));

    glBindVertexArray(0);
}

void MeshGeometry::cleanupGeometry() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
}