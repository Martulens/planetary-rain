#include "noise.h"
#include "glm/gtc/noise.hpp"

#include <cmath>
#include <iostream>
#include <ostream>
#include <set>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/gtx/compatibility.hpp"


float NoiseSettings::perlin(const glm::vec3& in){
    // To normalize into x in [0, 1];
    return 0.5f * (glm::perlin(in) + 1);
}

float NoiseSettings::rigidPerlin(const glm::vec3& in){
    // To normalize into x in [0, 1];
    return 1 - fabs(sin(perlin(in)));
}

// Inspired by: https://www.ronja-tutorials.com/post/027-layered-noise/
float NoiseSettings::computeNoise(const glm::vec3& in){
    float noise = 0;
    float factor = amplitude;
    float freq = frequency;

    for (int i = 0; i < octaves; i++){
        glm::vec3 point = in * freq + glm::vec3(i * offset);

        float local = (rigidPerlin(point) * factor)/(i + 1);

        noise += local;
        factor *= persistence;
        freq *= roughness;

    }

    return noise;
}


float Noise::computeAll(const glm::vec3& in){
    float out = 0.0f;
    size_t size = settings.size();

    for (size_t i = 0; i < size; i++){
        if(settings[i].shown)
            out += settings[i].computeNoise(in);
    }

    return out;
}
