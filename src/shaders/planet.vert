#version 330 core
#define NOISE_MAX 20

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


uniform bool wavesEnabled;
uniform float waveHeight;
uniform float waveLength;
uniform float waveSpeed;
uniform float waveOffset;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragColor;
out float visibility;
out float vHeight;
out float isOcean;

// === Simple hash-based noise ===
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

float turbulentPerlin(vec3 pos){
    return abs(snoise(pos));
}

float ridgidPerlin(vec3 pos, float off){
    float n = abs(snoise(pos));
    n = off - n;

    return n * n;
}

float computeNoise(vec3 pos, float amp, float f, int o, float p, float r, float off, float t){
    float noise = 0;
    float factor = amp;
    float freq = f;
    float oSize = off/o;

    float currOff = off;
    for (int i = 0; i < o; i++){
        vec3 point = pos * freq + vec3(i * currOff);
        float local = 0.0f;

        if(t == 0)
        local = (normalPerlin(point) * factor);
        else if(t == 1)
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

float computeAll(vec3 pos) {
    float elevation = 0.0;

    // Base terrain
    if (numNoises > 0 && shown[0]) {
        float continents = computeNoise(pos,
        amplitude[0], frequency[0], octaves[0],
        persistence[0], roughness[0], noiseOffset[0], type[0]);

        elevation += continents;
    }

    float landMask = smoothstep(oceanLevel, oceanLevel + 0.05, elevation);

    // Detail layers - each one weaker than the last
    float detailFalloff = 0.5;  // Each layer is half as strong
    float currentStrength = 0.08;

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

    // Gentle ocean floor
    elevation = max(elevation, oceanLevel);
    return elevation;
}

vec3 waveNormal(vec3 pos){
    vec3 dir = normalize(pos);
    vec2 waveDir = normalize(dir.xz);

    float time = waveSpeed * deltaTime;
    float steepness = 1.0;

    float phase = waveLength * dot(waveDir, pos.xz) + time;
    float cosFactor = cos(phase);
    float sinFactor = sin(phase);

    float nx = -waveDir.x * waveLength * waveHeight * cosFactor;
    float nz = -waveDir.y * waveLength * waveHeight * cosFactor;
    float ny = 1.0 - steepness * waveHeight * sinFactor;


    return normalize(vec3(nx, ny, nz));
}

// inspired by https://catlikecoding.com/unity/tutorials/flow/waves/
// https://gameidea.org/2023/12/01/3d-ocean-shader-using-gerstner-waves/
vec3 computeWave(vec3 pos){
    vec3 vertex = pos;
    vec3 dir = normalize(pos);

    float time = waveSpeed * deltaTime;
    float steepness = 1.0;
    float waveChar = steepness / waveLength;

    // Use two components of direction for the wave propagation
    vec2 waveDir = normalize(dir.xz);
    float phase = waveLength * dot(waveDir, vertex.xz) + time;

    float x = sin(vertex.x + waveChar * waveDir.x * cos(phase));
    float z = sin(vertex.z + waveChar * waveDir.y * cos(phase));
    float y = sin(vertex.y + waveHeight * sin(phase));

    vec3 wavePos = waveHeight * vec3(x, y, z);

    return wavePos;
}

float checkOcean(vec3 pos, float height){
    float outValue = 0;

    if(height == oceanLevel && wavesEnabled){
        vec3 wave = computeWave(pos);
        outValue = distance(wave, pos);
    }

    return outValue;
}

vec3 computeNormal(float vHeight, vec3 pos, vec3 normal){
    // fallback for the poles -> cross should not equal 0
    vec3 up = abs(normal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 T = cross(up, normal);
    vec3 B = cross(normal, T);

    float hCenter = vHeight;
    hCenter += checkOcean(pos, vHeight);

    vec3 posT = pos + epsilon * T;
    float hTangent   = computeAll(posT);
    hTangent += checkOcean(posT, hTangent);

    vec3 posB = pos + epsilon * B;
    float hBitangent = computeAll(posB);
    hBitangent += checkOcean(posB, hBitangent);

    // (tangets - center) are partial derivatives -> how steep the surface is in each dir
    vec3 dT = T * epsilon + normal * (hTangent - hCenter);
    vec3 dB = B * epsilon + normal * (hBitangent - hCenter);

    vec3 dNormal = normalize(cross(dT, dB));

    return dNormal;
}

void main() {
    vHeight = computeAll(position);

    vec3 perlinPosition = radius * position * (vHeight + 1);

    if(vHeight <= oceanLevel + epsilon){
        if (wavesEnabled){
            float value = 0.0;
            vec3 displace = computeWave(perlinPosition);
            perlinPosition += displace;
        }
    }

    if(vHeight == oceanLevel)
        isOcean = 1.0;
    else
        isOcean = 0.0;

    // TODO
    // can possibly lower the size of noises to optimize -> depending on detail
    vec3 outNormal = computeNormal(vHeight, position, normal);

    vec4 worldPosition = modelMatrix * vec4(perlinPosition, 1.0);
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