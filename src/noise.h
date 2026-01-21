#ifndef NOISE_H
#define NOISE_H
#include "glm/vec3.hpp"

// sources:
//  -> https://www.cs.cmu.edu/~112-s23/notes/student-tp-guides/Terrain.pdf
//  -> https://adrianb.io/2014/08/09/perlinnoise.html
//  -> https://en.wikipedia.org/wiki/Simplex_noise
// preferable due to lower time complexity
//  -> https://en.wikipedia.org/wiki/Perlin_noise
class Noise {
private:
    int p[512];
public:
    Noise();
    float perlin(glm::vec3 in, int repeat);
};


#endif //NOISE_H
