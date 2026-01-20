//
// Created by martul3ns on 1/20/26.
//

#include "sphere.h"

Sphere::Sphere(glm::vec3 position, int radius, int detail, ModelTexture* texture, ShaderProgram* shader):
Object(position, generateGeometry(radius, detail), shader, texture){};

MeshGeometry* Sphere::generateGeometry(float radius, int detail){
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
            vertex.position = glm::vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
            vertex.normal = vertex.position;
            vertex.texCoord = glm::vec2(1.0f - (float)lon / lonSegments, 1.0f - (float)lat / latSegments);

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
