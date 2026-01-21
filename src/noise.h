#ifndef NOISE_H
#define NOISE_H


// Code inspired and created consecutively based on:
// -> https://github.com/SebLague/Procedural-Landmass-Generation
// -> https://garagefarm.net/blog/perlin-noise-implementation-procedural-generation-and-simplex-noise
// -> https://rtouti.github.io/graphics/perlin-noise-algorithm
// -> https://www.redblobgames.com/maps/terrain-from-noise/

#include "glm/glm.hpp"

class PerlinNoise {
private:
    int perm[512];

    float fade(float t);
    float lerp(float a, float b, float t);
    float grad(int hash, float x, float y, float z);

public:
    PerlinNoise(unsigned int seed = 0);

    float noise(glm::vec3 pos);
    float noise(float x, float y, float z);

    float octaveNoise(glm::vec3 pos, int octaves, float persistence = 0.5f);
};

#endif
