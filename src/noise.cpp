// noise.cpp
#include "noise.h"
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>

PerlinNoise::PerlinNoise(unsigned int seed) {
    std::iota(perm, perm + 256, 0);
    
    // Shuffle using seed
    std::default_random_engine engine(seed);
    std::shuffle(perm, perm + 256, engine);
    
    // Duplicate for overflow handling
    for (int i = 0; i < 256; ++i) {
        perm[256 + i] = perm[i];
    }
}

float PerlinNoise::fade(float t) {
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float PerlinNoise::lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y, float z) {
    // Convert low 4 bits of hash into 12 gradient directions
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float PerlinNoise::noise(float x, float y, float z) {
    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;
    int Z = static_cast<int>(floor(z)) & 255;
    
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);
    
    // Compute fade curves
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);
    
    // Hash coordinates of cube corners
    int A  = perm[X] + Y;
    int AA = perm[A] + Z;
    int AB = perm[A + 1] + Z;
    int B  = perm[X + 1] + Y;
    int BA = perm[B] + Z;
    int BB = perm[B + 1] + Z;
    
    // Blend results from 8 corners
    return lerp(
        lerp(
            lerp(grad(perm[AA], x, y, z),
                 grad(perm[BA], x - 1, y, z), u),
            lerp(grad(perm[AB], x, y - 1, z),
                 grad(perm[BB], x - 1, y - 1, z), u), v),
        lerp(
            lerp(grad(perm[AA + 1], x, y, z - 1),
                 grad(perm[BA + 1], x - 1, y, z - 1), u),
            lerp(grad(perm[AB + 1], x, y - 1, z - 1),
                 grad(perm[BB + 1], x - 1, y - 1, z - 1), u), v), w);
}

float PerlinNoise::noise(glm::vec3 pos) {
    return noise(pos.x, pos.y, pos.z);
}

float PerlinNoise::octaveNoise(glm::vec3 pos, int octaves, float persistence) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; ++i) {
        total += noise(pos * frequency) * amplitude;
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    
    return total / maxValue;
}