#include "sphere.h"

#include "config.h"

Sphere::Sphere(glm::vec3 position,
               float radius,
               int detail,
               const std::vector<NoiseSettings>& sets,
               bool show,
               std::shared_ptr<ModelTexture> texture,
               std::shared_ptr<ShaderProgram> shader)
    : mRadius(radius), mDetail(detail), mShowTerrain(show) {

    this->mPosition = position;
    this->mTexture = texture;
    this->mShader = shader;

    updateModelMatrix();
    mNoise = Noise(sets);
    this->mGeometry = cubeSphere();
}

std::shared_ptr<MeshGeometry> Sphere::uvSphere() {
    // mDetail: 1=low, 2=medium, 3=high, 4=ultra
    int latSegments = 8 * mDetail;
    int lonSegments = 16 * mDetail;

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
            vertex.position = mRadius * glm::vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
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
    return std::make_shared<MeshGeometry>(vertices, indices);
}

// contributions taken from:
// -> https://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html?lr=1
float projectToSphere(float in, float a, float b){
    return in*sqrt(1 - (a*a)/2 - (b*b)/2 + ((a*a)*(b*b))/3);
}

std::shared_ptr<MeshGeometry> Sphere::cubeSphere(){
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for(int face = 0; face < 6; ++face){

        glm::vec3 a = cubeVertices[face*4];
        glm::vec3 b = cubeVertices[face*4+1];
        glm::vec3 d = cubeVertices[face*4+3];

        glm::vec3 u = (b - a);
        glm::vec3 iDiff = glm::vec3(u.x/(float)mDetail, u.y/(float)mDetail, u.z/(float)mDetail);
        glm::vec3 v = (d - a);
        glm::vec3 jDiff = glm::vec3(v.x/(float)mDetail, v.y/(float)mDetail, v.z/(float)mDetail);

        /*
        std::cout << "[CUBE] Face: " << face << std::endl;
        std::cout << "[CUBE] Vertices: " << std::endl;
        std::cout << "  -> " << a.x << ", " << a.y << ", " << a.z << std::endl;
        std::cout << "  -> " << b.x << ", " << b.y << ", " << b.z << std::endl;
        std::cout << "  -> " << c.x << ", " << c.y << ", " << c.z << std::endl;
        */

        /*
        std::cout << "[CUBE] Diffs: " << std::endl;
        std::cout << "  -> " << iDiff.x << ", " << iDiff.y << ", " << iDiff.z << std::endl;
        std::cout << "  -> " << jDiff.x << ", " << jDiff.y << ", " << jDiff.z << std::endl;
        */

        for(int j = 0; j <= mDetail; ++j){
            for(int i = 0; i <= mDetail; ++i){
                Vertex v;
                glm::vec3 cubePos = (a + (float)i*iDiff + (float)j*jDiff);

                float x = projectToSphere(cubePos.x, cubePos.y, cubePos.z);
                float y = projectToSphere(cubePos.y, cubePos.x, cubePos.z);
                float z = projectToSphere(cubePos.z, cubePos.x, cubePos.y);

                glm::vec3 spherePos = glm::vec3(x, y, z);
                float height = mNoise.computeAll(spherePos);

                std::cout << "height = " << height << std::endl;

                if(height < 1)
                    height = 1.0f;

                std::cout << "[CUBE] color: " << v.color.r << ", " << v.color.g << ", " << v.color.b << std::endl;
                v.position = mRadius*spherePos*(height+1);
                v.normal = glm::normalize(v.position);

                vertices.push_back(v);

                //std::cout << "[CUBE] " << i+j*(mDetail+1) << " Position: " << v.position.x << ", " << v.position.y << ", " << v.position.z << std::endl;
            }
        }
    }

    for(int face = 0; face < 6; ++face){
        int baseIndex = face * (mDetail + 1) * (mDetail + 1);
        int numSquares = mDetail * mDetail;

        for(int i = 0; i < numSquares; ++i){
            int line = i / mDetail;

            int a = baseIndex + i + line;
            int b = a + 1;
            int d = a + mDetail + 1;
            int c = d + 1;

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    return  std::make_shared<MeshGeometry>(vertices, indices);
}