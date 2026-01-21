#include "sphere.h"

#include <complex>

Sphere::Sphere(glm::vec3 position, float r, int det, ModelTexture* texture, ShaderProgram* shader)
    : Object(position, cubeSphere(r, det), shader, texture), radius(r), detail(det) {
    updateModelMatrix();
}

MeshGeometry* Sphere::uvSphere(float radius, int detail) {
    // detail: 1=low, 2=medium, 3=high, 4=ultra
    int latSegments = 8 * detail;
    int lonSegments = 16 * detail;

    std::vector<Vertex> vertices;
    for (int lat = 0; lat <= latSegments; ++lat) {
        float theta = lat * glm::pi<float>() / latSegments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= lonSegments; ++lon) {
            float phi = lon * 2.0f * glm::pi<float>() / lonSegments;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            Vertex vertex;
            vertex.position = radius * glm::vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
            vertex.normal = glm::normalize(glm::vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta));
            vertex.texCoord = glm::vec2(1.0f - (float)lon / lonSegments, 1.0f - (float)lat / latSegments);
            vertex.color = glm::vec3(1.0f);

            vertices.push_back(vertex);
        }
    }

    std::vector<unsigned int> indices;
    for (int lat = 0; lat < latSegments; ++lat) {
        for (int lon = 0; lon < lonSegments; ++lon) {
            int first = lat * (lonSegments + 1) + lon;
            int second = first + lonSegments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    return new MeshGeometry(vertices, indices);
}

glm::vec3 cubeVertices[24] = {
    // +x
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),

    // -x
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),

    // +y
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),

    // -y
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),

    // +z
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),

    // -z
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),
};

glm::vec3 cubeNormals[6] = {
    // x
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    // y
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    // z
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f)
};

// contributions taken from:
// -> https://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html?lr=1
float projectToSphere(float in, float a, float b){
    return in*sqrt(1 - (a*a)/2 - (b*b)/2 + ((a*a)*(b*b))/3);
}

MeshGeometry* Sphere::cubeSphere(float radius, int detail){
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for(int face = 0; face < 6; ++face){
        glm::vec3 normal = cubeNormals[face];

        glm::vec3 a = cubeVertices[face*4];
        glm::vec3 b = cubeVertices[face*4+1];
        glm::vec3 d = cubeVertices[face*4+3];

        glm::vec3 u = (b - a);
        glm::vec3 iDiff = glm::vec3(u.x/(float)detail, u.y/(float)detail, u.z/(float)detail);
        glm::vec3 v = (d - a);
        glm::vec3 jDiff = glm::vec3(v.x/(float)detail, v.y/(float)detail, v.z/(float)detail);

        /*
        std::cout << "[CUBE] Face: " << face << std::endl;
        std::cout << "[CUBE] Vertices: " << std::endl;
        std::cout << "  -> " << a.x << ", " << a.y << ", " << a.z << std::endl;
        std::cout << "  -> " << b.x << ", " << b.y << ", " << b.z << std::endl;
        std::cout << "  -> " << c.x << ", " << c.y << ", " << c.z << std::endl;
        */

        std::cout << "[CUBE] Diffs: " << std::endl;
        std::cout << "  -> " << iDiff.x << ", " << iDiff.y << ", " << iDiff.z << std::endl;
        std::cout << "  -> " << jDiff.x << ", " << jDiff.y << ", " << jDiff.z << std::endl;

        glm::vec3 jPlus = glm::vec3(0.0f);
        for(int j = 0; j <= detail; ++j){
            glm::vec3 iPlus = glm::vec3(0.0f);
            for(int i = 0; i <= detail; ++i){
                Vertex v;
                glm::vec3 cubePos = (a + (float)i*iDiff + (float)j*jDiff);

                float x = projectToSphere(cubePos.x, cubePos.y, cubePos.z);
                float y = projectToSphere(cubePos.y, cubePos.x, cubePos.z);
                float z = projectToSphere(cubePos.z, cubePos.x, cubePos.y);

                glm::vec3 spherePos = glm::vec3(x, y, z);
                glm::vec3 sphereNormal = glm::normalize(spherePos - position);

                v.position = radius*spherePos;
                v.normal = sphereNormal;

                vertices.push_back(v);

                std::cout << "[CUBE] " << i+j*(detail+1) << " Position: " << v.position.x << ", " << v.position.y << ", " << v.position.z << std::endl;
            }
        }
    }

    for(int face = 0; face < 6; ++face){
        int baseIndex = face * (detail + 1) * (detail + 1);
        int numSquares = detail * detail;

        for(int i = 0; i < numSquares; ++i){
            int line = i / detail;

            int a = baseIndex + i + line;
            int b = a + 1;
            int d = a + detail + 1;
            int c = d + 1;

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    return new MeshGeometry(vertices, indices);
}