#include "sphere.h"
#include "noise.h"

Sphere::Sphere(glm::vec3 position, float r, int det, ModelTexture* texture, ShaderProgram* shader, Terrain terrain)
    : Object(position, cubeSphere(r, det, rand()), shader, texture), radius(r), detail(det), terrain(terrain)
{
    updateModelMatrix();
}

Sphere::Sphere(glm::vec3 position, float r, int det, ModelTexture* texture, ShaderProgram* shader)
    : Object(position, UVSphere(r, det), shader, texture), radius(r), detail(det)
{
    updateModelMatrix();
}

MeshGeometry* Sphere::UVSphere(float radius, int detail){
    // detail: 1=low, 2=medium, 3=high, 4=ultra
    int latSegments = 8 * detail;
    int lonSegments = 16 * detail;

    std::vector<Vertex> vertices;
    for (int lat = 0; lat <= latSegments; ++lat)
    {
        float theta = lat * glm::pi<float>() / latSegments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= lonSegments; ++lon)
        {
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
    for (int lat = 0; lat < latSegments; ++lat)
    {
        for (int lon = 0; lon < lonSegments; ++lon)
        {
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

// code inspired by:
// -> https://github.com/caosdoar/spheres
//
MeshGeometry* Sphere::cubeSphere(float radius, int detail, int seed) {
    // detail = subdivisions per face edge (e.g., 8 means 8x8 grid per face)
    int resolution = pow(2, detail);
    PerlinNoise noise = PerlinNoise(42);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // 6 face directions: +X, -X, +Y, -Y, +Z, -Z
    glm::vec3 directions[6] = {
        glm::vec3(1, 0, 0),  glm::vec3(-1, 0, 0),
        glm::vec3(0, 1, 0),  glm::vec3(0, -1, 0),
        glm::vec3(0, 0, 1),  glm::vec3(0, 0, -1)
    };

    for (int face = 0; face < 6; ++face) {
        glm::vec3 localUp = directions[face];
        glm::vec3 axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
        glm::vec3 axisB = glm::cross(localUp, axisA);

        unsigned int baseIndex = static_cast<unsigned int>(vertices.size());

        for (int y = 0; y <= resolution; ++y) {
            for (int x = 0; x <= resolution; ++x) {
                glm::vec2 percent = glm::vec2(x, y) / static_cast<float>(resolution);

                glm::vec3 pointOnCube = localUp
                    + (percent.x - 0.5f) * 2.0f * axisA
                    + (percent.y - 0.5f) * 2.0f * axisB;

                glm::vec3 pointOnSphere = glm::normalize(pointOnCube);

                // Sample noise at this point on the sphere
                float elevation = noise.octaveNoise(pointOnSphere * 2.0f, 4, 0.5f);

                float minHeight = terrain.minValue;
                float maxHeight = terrain.maxValue;
                float height = radius * (minHeight + (elevation + 1.0f) * 0.5f * (maxHeight - minHeight));

                Vertex v;
                v.position = pointOnSphere * height;
                v.normal = pointOnSphere;
                v.texCoord = percent;
                v.color = glm::vec3(1.0f);

                vertices.push_back(v);
            }
        }

        for (int y = 0; y < resolution; ++y) {
            for (int x = 0; x < resolution; ++x) {
                unsigned int i = baseIndex + y * (resolution + 1) + x;

                // First triangle
                indices.push_back(i);
                indices.push_back(i + resolution + 1);
                indices.push_back(i + 1);

                // Second triangle
                indices.push_back(i + 1);
                indices.push_back(i + resolution + 1);
                indices.push_back(i + resolution + 2);
            }
        }

        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int i0 = indices[i];
            unsigned int i1 = indices[i + 1];
            unsigned int i2 = indices[i + 2];

            glm::vec3 v0 = vertices[i0].position;
            glm::vec3 v1 = vertices[i1].position;
            glm::vec3 v2 = vertices[i2].position;

            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 faceNormal = glm::cross(edge1, edge2);

            // Accumulate to each vertex (smooth shading)
            vertices[i0].normal += faceNormal;
            vertices[i1].normal += faceNormal;
            vertices[i2].normal += faceNormal;
        }

        for (Vertex& v : vertices) {
            v.normal = glm::normalize(v.normal);
        }
    }

    return new MeshGeometry(vertices, indices);
}

float Sphere::getElevation(PerlinNoise noise, glm::vec3 pointOnSphere) {
    float value = 0.0f;
    float frequency = terrain.baseRoughness;
    float amplitude = 1.0f;

    for (int i = 0; i < terrain.octaves; ++i) {
        float v = noise.noise((pointOnSphere + terrain.center) * frequency);
        value += (v + 1.0f) * 0.5f * amplitude;
        frequency *= terrain.roughness;
        amplitude *= terrain.persistence;
    }

    value = glm::max(0.0f, value - terrain.minValue);
    return value * terrain.strength;
}