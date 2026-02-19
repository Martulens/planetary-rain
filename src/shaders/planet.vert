#version 330 core
#define NOISE_MAX 20
#define WAVE_MAX 10
#define PI 3.14159265359

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float deltaTime;
uniform float epsilon;
uniform float oceanLevel;

uniform float density;
uniform float gradient;
uniform float radius;

uniform int numNoises;
uniform int octaves[NOISE_MAX];
uniform int type[NOISE_MAX];

uniform bool shown[NOISE_MAX];

uniform float frequency[NOISE_MAX];
uniform float amplitude[NOISE_MAX];
uniform float persistence[NOISE_MAX];
uniform float roughness[NOISE_MAX];
uniform float noiseOffset[NOISE_MAX];

uniform int numWaves;
uniform bool wavesEnabled;

// Per-wave parameters
uniform float waveAmplitude[WAVE_MAX];    // A_i
uniform float waveFrequency[WAVE_MAX];    // omega_i
uniform float waveSpeed[WAVE_MAX];        // phi_i (phase speed)
uniform float waveSteepness[WAVE_MAX];    // Q_i (0 to 1)
uniform vec3  waveOrigin[WAVE_MAX];       // o_i - unit vector pointing to wave origin on sphere

// Smoothstep fade parameters to prevent loops at origin/antipode
uniform float waveFadeE0;
uniform float waveFadeE1;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragColor;
out float visibility;
out float vHeight;
out float isOcean;

// ============================================================
// Simplex Noise (unchanged)
// ============================================================
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 mod289(vec4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 permute(vec4 x) { return mod289(((x * 34.0) + 1.0) * x); }
vec4 taylorInvSqrt(vec4 r) { return 1.79284291400159 - 0.85373472095314 * r; }

float snoise(vec3 v) {
    const vec2 C = vec2(1.0 / 6.0, 1.0 / 3.0);
    const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);

    vec3 i  = floor(v + dot(v, C.yyy));
    vec3 x0 = v - i + dot(i, C.xxx);

    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min(g.xyz, l.zxy);
    vec3 i2 = max(g.xyz, l.zxy);

    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy;
    vec3 x3 = x0 - D.yyy;

    i = mod289(i);
    vec4 p = permute(permute(permute(
    i.z + vec4(0.0, i1.z, i2.z, 1.0))
    + i.y + vec4(0.0, i1.y, i2.y, 1.0))
    + i.x + vec4(0.0, i1.x, i2.x, 1.0));

    float n_ = 0.142857142857;
    vec3 ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);
    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_);

    vec4 x = x_ * ns.x + ns.yyyy;
    vec4 y = y_ * ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4(x.xy, y.xy);
    vec4 b1 = vec4(x.zw, y.zw);

    vec4 s0 = floor(b0) * 2.0 + 1.0;
    vec4 s1 = floor(b1) * 2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

    vec3 p0 = vec3(a0.xy, h.x);
    vec3 p1 = vec3(a0.zw, h.y);
    vec3 p2 = vec3(a1.xy, h.z);
    vec3 p3 = vec3(a1.zw, h.w);

    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2,p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m*m, vec4(dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3)));
}

float normalPerlin(vec3 pos) {
    return 0.5 * (snoise(pos) + 1.0);
}

float turbulentPerlin(vec3 pos) {
    return abs(snoise(pos));
}

float ridgidPerlin(vec3 pos, float off) {
    float n = abs(snoise(pos));
    n = off - n;
    return n * n;
}

float computeNoise(vec3 pos, float amp, float f, int o, float p, float r, float off, float t) {
    float noise = 0.0;
    float factor = amp;
    float freq = f;
    float oSize = off / o;
    float currOff = off;

    for (int i = 0; i < o; i++) {
        vec3 point = pos * freq + vec3(i * currOff);
        float local = 0.0;

        if (t == 0)
        local = normalPerlin(point) * factor;
        else if (t == 1)
        local = turbulentPerlin(point) * factor;
        else
        local = ridgidPerlin(point, currOff) * factor;

        noise += local;
        factor *= p;
        freq *= r;
        currOff -= oSize;
    }

    return noise;
}

// Spherical Gerstner Waves (Paper Equations 7, 8, 10, 11)
// https://cescg.org/wp-content/uploads/2018/04/Michelic-Real-Time-Rendering-of-Procedurally-Generated-Planets-2.pdf

// Equation 7: compute tangent direction and geodesic distance for wave i
// di = direction on tangent plane pointing toward -oi (away from wave origin)
// li = arc-length distance from wave origin to vertex
void waveParams(vec3 v, vec3 oi, out vec3 di, out float li) {
    // Geodesic distance: arc length = acos(v . oi) * r
    // We use r = 1 here (unit sphere), actual radius applied later
    float cosAngle = clamp(dot(v, oi), -1.0, 1.0);
    li = acos(cosAngle);

    // Direction on tangent plane: double cross product
    // di = v x ((v - oi) x v)
    vec3 diff = v - oi;
    vec3 inner = cross(diff, v);
    di = cross(v, inner);

    float len = length(di);
    if (len > 1e-6) {
        di = di / len;
    } else {
        // At the origin or antipode -> wrongly displaced -> direction is degenerate
        // Pick an arbitrary tangent
        vec3 up = abs(v.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
        di = normalize(cross(v, up));
    }
}

// Equation 11: smoothstep fade Q near origin and antipode to prevent loops
float fadeSteepness(float Qi, vec3 v, vec3 oi, float e0, float e1) {
    float alignment = 1.0 - abs(dot(v, oi));
    return Qi * smoothstep(e0, e1, alignment);
}

// Equation 8: spherical Gerstner wave displacement
// Equation 10: analytical normal
// Returns the displaced position and computes the analytical normal
vec3 computeSphericalWaves(vec3 pos, float seaRadius, out vec3 waveNormal) {
    vec3 v = normalize(pos);

    // Accumulate displacement components

    // along v
    vec3 radialDisp = vec3(0.0);
    // along di
    vec3 tangentDisp = vec3(0.0);

    // Accumulate normal components (Eq. 10)
    float normalRadialSum = 0.0;
    vec3  normalTangentSum = vec3(0.0);

    for (int i = 0; i < numWaves && i < WAVE_MAX; ++i) {
        vec3 di;
        float li;
        waveParams(v, waveOrigin[i], di, li);

        // Scale li by sea-level radius for proper wavelength
        li *= seaRadius;

        float Ai = waveAmplitude[i];
        float wi = waveFrequency[i];
        float phi_i = waveSpeed[i];
        float Qi = waveSteepness[i];

        // Fade steepness near origin/antipode (Eq. 11)
        Qi = fadeSteepness(Qi, v, waveOrigin[i], waveFadeE0, waveFadeE1);

        float phase = wi * li + phi_i * deltaTime;
        float sinPhase = sin(phase);
        float cosPhase = cos(phase);

        // Equation 8: displacement
        radialDisp  += v * (Ai * sinPhase);
        tangentDisp += di * (Qi * Ai * cosPhase);

        // Equation 10: normal components
        normalRadialSum  += Qi * Ai * wi * sinPhase;
        normalTangentSum += di * Ai * wi * cosPhase;
    }

    // Final displaced position (Eq. 8):
    // P_s = v*r + v*sum(Ai*sin(...)) + sum(Qi*Ai*cos(...)*di)
    vec3 displaced = v * seaRadius + radialDisp + tangentDisp;

    // Analytical normal (Eq. 10):
    // n_s = v - v*sum(Qi*Ai*wi*sin(...)) - sum(di*Ai*wi*cos(...))
    waveNormal = normalize(v * (1.0 - normalRadialSum) - normalTangentSum);

    return displaced;
}

float computeTerrainElevation(vec3 pos) {
    float elevation = 0.0;

    // Base terrain
    if (numNoises > 0 && shown[0]) {
        float continents = computeNoise(pos,
        amplitude[0], frequency[0], octaves[0],
        persistence[0], roughness[0], noiseOffset[0], type[0]);
        elevation += continents;
    }

    float landMask = smoothstep(oceanLevel, oceanLevel + 0.05, elevation);

    // Detail layers
    float currentStrength = 0.08;
    float detailFalloff = 0.5;

    for (int i = 1; i < numNoises && i < NOISE_MAX; ++i) {
        if (shown[i]) {
            float detail = computeNoise(pos,
            amplitude[i], frequency[i], octaves[i],
            persistence[i], roughness[i], noiseOffset[i], type[i]);

            float userScale = clamp(amplitude[i], 0.0, 1.0);
            elevation += detail * landMask * currentStrength * userScale;
            currentStrength *= detailFalloff;
        }
    }

    return elevation;
}

// Terrain normal via finite differences -> only for land
vec3 computeTerrainNormal(float hCenter, vec3 pos, vec3 nrm) {
    vec3 up = abs(nrm.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 T = normalize(cross(up, nrm));
    vec3 B = cross(nrm, T);

    vec3 posT = pos + epsilon * T;
    float hT = computeTerrainElevation(posT);

    vec3 posB = pos + epsilon * B;
    float hB = computeTerrainElevation(posB);

    vec3 dT = T * epsilon + nrm * (hT - hCenter);
    vec3 dB = B * epsilon + nrm * (hB - hCenter);

    return normalize(cross(dT, dB));
}

// inspired by https://catlikecoding.com/unity/tutorials/flow/waves/
// https://gameidea.org/2023/12/01/3d-ocean-shader-using-gerstner-waves/
// https://www.desmos.com/calculator/o8fgriyavw
// https://claude.ai/chat/7f14301e-d3f7-4f11-b5ea-9956b95899a3
void main() {
    vec3 pos = position;
    vec3 unitPos = normalize(pos);

    // Compute terrain elevation on unit sphere
    float elevation = computeTerrainElevation(pos);

    vec3 outNormal;
    vec3 displacedPos;

    if (elevation <= oceanLevel) {
        // Ocean using spherical Gerstner waves
        isOcean = 1.0;
        vHeight = oceanLevel;

        float seaRadius = radius * (oceanLevel + 1.0);

        vec3 waveNormal;
        displacedPos = computeSphericalWaves(pos, seaRadius, waveNormal);

        // Wave normal is already analytical —> no finite differences needed
        outNormal = waveNormal;

    } else {
        // Land is displaced by elevation -> compute normal via finite differences
        isOcean = 0.0;
        vHeight = elevation;

        displacedPos = unitPos * radius * (elevation + 1.0);

        outNormal = computeTerrainNormal(elevation, pos, normal);
    }

    vec4 worldPosition = modelMatrix * vec4(displacedPos, 1.0);
    vec4 cameraPos = viewMatrix * worldPosition;
    float distanceToCamera = length(cameraPos.xyz);

    visibility = exp(-pow(distanceToCamera * density, gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    fragPosition = worldPosition.xyz;
    fragTexCoord = texCoord;
    fragColor = color;

    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    fragNormal = normalize(normalMatrix * outNormal);

    gl_Position = projectionMatrix * viewMatrix * worldPosition;
}