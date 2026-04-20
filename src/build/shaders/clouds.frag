#version 330 core

in vec3 fragPosition;

uniform vec3 cameraPosition;
uniform vec3 planetCenter;

uniform float innerRadius;
uniform float outerRadius;

uniform vec3 sunDirection;   // normalized, points from cloud toward sun
uniform vec3 sunColor;
uniform vec3 cloudColor;

uniform float coverage;
uniform float cloudDensity;
uniform float cloudSpeed;
uniform float noiseScale;
uniform float cloudAlpha;

uniform float bandStrength;
uniform float bandFreq;
uniform float deltaTime;

uniform vec3 skyColor;

out vec4 outColor;

// Simplex noise (same kernel as planet.vert)
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
    p0 *= norm.x; p1 *= norm.y; p2 *= norm.z; p3 *= norm.w;

    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m*m, vec4(dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3)));
}

// fBm: 3 octaves, drifted by time.
// Optional latitudinal band modulation lets gas giants look layered.
float cloudDensityAt(vec3 p) {
    vec3 np = p * noiseScale + vec3(deltaTime * cloudSpeed, 0.0, 0.0);
    float n = 0.0;
    float amp = 1.0;
    float totalAmp = 0.0;
    for (int i = 0; i < 3; i++) {
        n += snoise(np) * amp;
        totalAmp += amp;
        np *= 2.1;
        amp *= 0.5;
    }
    n = (n / totalAmp) * 0.5 + 0.5; // normalize to 0..1
    float d = max(0.0, n - (1.0 - coverage));

    // Latitude banding: sin wave along y gives horizontal stripes of varying density
    if (bandStrength > 0.0) {
        float bands = 0.5 + 0.5 * sin(p.y * bandFreq);
        d *= mix(1.0, bands, bandStrength);
    }
    return d;
}

// Ray vs sphere. Writes the two intersection distances
void raySphere(vec3 ro, vec3 rd, vec3 ce, float ra, out float t1, out float t2) {
    vec3 oc = ro - ce;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - ra * ra;
    float h = b * b - c;
    if (h < 0.0) { t1 = t2 = -1.0; return; }
    h = sqrt(h);
    t1 = -b - h;
    t2 = -b + h;
}

void main() {
    if (coverage <= 0.001) discard;

    vec3 rayDir = normalize(fragPosition - cameraPosition);

    // work out how far we can march before we leave the shell
    float to1, to2, ti1, ti2;
    raySphere(fragPosition, rayDir, planetCenter, outerRadius, to1, to2);
    raySphere(fragPosition, rayDir, planetCenter, innerRadius, ti1, ti2);

    float maxDist = max(to2, 0.0);
    if (ti1 > 0.001)
        maxDist = min(maxDist, ti1);

    if (maxDist < 0.001)
        discard;

    // Adaptive step count: roughly one sample per TARGET_STEP world units,
    // clamped so thin shells stay readable and thick shells stay cheap.
    const int   MAX_STEPS   = 16;
    const float TARGET_STEP = 0.22;
    int   numSteps = clamp(int(maxDist / TARGET_STEP), 4, MAX_STEPS);
    float stepSize = maxDist / float(numSteps);

    float transmittance = 1.0;
    vec3 lightAccum = vec3(0.0);
    vec3 pos = fragPosition;

    for (int i = 0; i < MAX_STEPS; i++) {
        if (i >= numSteps) break;

        float d = cloudDensityAt(pos);
        if (d > 0.0) {
            float extinction = d * cloudDensity * stepSize;

            // Radial out of the planet approximates the local up
            vec3 radial = normalize(pos - planetCenter);
            float sunDot = max(dot(radial, sunDirection), 0.0);
            vec3 illum = cloudColor * sunColor * (0.25 + 0.75 * sunDot);

            // Beer-Lambert
            lightAccum += illum * transmittance * (1.0 - exp(-extinction));
            transmittance *= exp(-extinction);

            if (transmittance < 0.01) break;
        }
        pos += rayDir * stepSize;
    }

    // Softened alpha with a ceiling below 1 so the surface stays visible.
    float opacity = pow(1.0 - transmittance, 1.4);
    float alpha = min(opacity * cloudAlpha, 0.80);
    outColor = vec4(lightAccum, alpha);
}
