#pragma once

#ifndef NOISE_H
#define NOISE_H
#include <vector>

#include "glm/vec3.hpp"

// TODO:
// Move all into vertex shader

// sources:
//  -> https://www.cs.cmu.edu/~112-s23/notes/student-tp-guides/Terrain.pdf
//  -> https://adrianb.io/2014/08/09/perlinnoise.html
//  -> https://en.wikipedia.org/wiki/Simplex_noise
// preferable due to lower time complexity
//  -> https://en.wikipedia.org/wiki/Perlin_noise
class NoiseSettings{
private:
public:

    // TODO remake
    bool shown = true;
    int octaves = 2;
    float frequency = 1;
    float amplitude = 1;
    float persistence = 1;
    float roughness = 1;
    float offset = 0.72354;

    NoiseSettings() = default;
    NoiseSettings(int o, float f, float a, float p ): octaves(o), frequency(f), amplitude(a), persistence(p){};
    NoiseSettings(bool s, int o, float f, float a, float p, float r, float off)
    : shown(s), octaves(o), frequency(f), amplitude(a), persistence(p), roughness(r), offset(off){};

    float perlin(const glm::vec3& in);
    float computeNoise(const glm::vec3& in);
    float rigidPerlin(const glm::vec3& in);

    // === GETTERS
    int getOctaves(){ return octaves; }
    float getFrequency(){ return frequency; }
    float getAmplitude(){ return amplitude; }
    float getPersistence(){ return persistence; }
};

class Noise {
private:
    std::vector<NoiseSettings> mSettings;

public:
    Noise() = default;
    Noise(const std::vector<NoiseSettings>& set): mSettings(set) {};

    float perlin(const glm::vec3& in);
    float computeNoise(const glm::vec3& in, int i );
    float computeAll(const glm::vec3& in);
};

#endif //NOISE_H
