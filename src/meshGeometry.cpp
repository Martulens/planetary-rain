#include "meshGeometry.h"

MeshGeometry::MeshGeometry(std::string& fileName) {
    loadModel(fileName);
}

MeshGeometry::MeshGeometry(const char* fileName) {
    std::string inputPath(fileName);
    loadModel(inputPath);
}

MeshGeometry::MeshGeometry(std::vector<Vertex> verts, std::vector<unsigned int> inds)
    : mIndices(std::move(inds)), mVertices(std::move(verts)) {
    mNumVertices = static_cast<unsigned int>(mVertices.size());
    mNumTriangles = static_cast<unsigned int>(mIndices.size() / 3);
    loadVAO();
}

MeshGeometry::~MeshGeometry() {
    cleanupGeometry();
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
    mScene = mImporter.ReadFile(fileName,
        aiProcess_Triangulate | aiProcess_FlipUVs |
        aiProcess_GenNormals | aiProcess_LimitBoneWeights);

    if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << mImporter.GetErrorString() << std::endl;
        return;
    }

    processNodeHierarchy(mScene->mRootNode, glm::mat4(1.0f));

    mVertices.clear();
    mIndices.clear();

    unsigned int vertexOffset = 0;

    for (unsigned int m = 0; m < mScene->mNumMeshes; ++m) {
        aiMesh* mesh = mScene->mMeshes[m];

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            Vertex vertex;
            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            vertex.texCoord = mesh->HasTextureCoords(0)
                ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f);

            vertex.color = glm::vec3(1.0f);
            mMaxY = std::max(mMaxY, mesh->mVertices[i].y);

            mVertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                mIndices.push_back(face.mIndices[j] + vertexOffset);
            }
        }

        vertexOffset += mesh->mNumVertices;
    }

    mNumVertices = static_cast<unsigned int>(mVertices.size());
    mNumTriangles = static_cast<unsigned int>(mIndices.size() / 3);

    loadVAO();
}

void MeshGeometry::loadVAO() {
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    glGenBuffers(1, &mVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW);

    GLsizei stride = sizeof(Vertex);

    // position - location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, position));

    // texCoord - location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texCoord));

    // normal - location 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));

    // color - location 3
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, color));

    glBindVertexArray(0);
}

void MeshGeometry::cleanupGeometry() {
    if (mVao != 0) {
        glDeleteVertexArrays(1, &mVao);
        mVao = 0;
    }
    if (mEbo != 0) {
        glDeleteBuffers(1, &mEbo);
        mEbo = 0;
    }
    if (mVbo != 0) {
        glDeleteBuffers(1, &mVbo);
        mVbo = 0;
    }
}