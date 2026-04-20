#pragma once

#ifndef NOISE_H
#define NOISE_H

#include <vector>

struct NoiseSettings {
    bool  shown       = true;
    int   type        = 0;     // 0: fBm, 1: turbulent, 2: ridged
    int   octaves     = 2;
    float frequency   = 1.0f;
    float amplitude   = 1.0f;
    float persistence = 1.0f;
    float roughness   = 1.0f;
    float offset      = 0.72354f;

    NoiseSettings() = default;
    NoiseSettings(int o, float f, float a, float p)
        : octaves(o), frequency(f), amplitude(a), persistence(p) {}
    NoiseSettings(bool s, int o, float f, float a, float p, float r, float off)
        : shown(s), type(0), octaves(o), frequency(f), amplitude(a),
          persistence(p), roughness(r), offset(off) {}
};

class Noise {
public:
    Noise() = default;
    explicit Noise(std::vector<NoiseSettings> set) : mSettings(std::move(set)) {}

    const std::vector<NoiseSettings>& getSettings() const { return mSettings; }

private:
    std::vector<NoiseSettings> mSettings;
};

#endif // NOISE_H
