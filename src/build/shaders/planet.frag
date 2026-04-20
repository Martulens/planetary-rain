#version 330 core

#define LIGHTS_MAX 20

// === INPUTS FROM VERTEX SHADER ===
in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragColor;
in float visibility;
in float vHeight;
in float isOcean;

// === UNIFORMS ===
uniform vec3 baseColor;   // mid-elevation terrain
uniform vec3 colorLow;    // low-elevation / shore terrain
uniform vec3 colorHigh;   // high-elevation / peak terrain
uniform float pd;           // diffuse coefficient
uniform float ps;           // specular coefficient
uniform float ns;           // shininess exponent

// -> reflection / refraction
uniform float reflectivity;
uniform float ior;          // index of refraction (>1 enables refractive path)
uniform float transparency; // 0 opaque -> 1 fully refractive

uniform samplerCube skyboxSampler;
uniform samplerCube envMapSampler;
uniform bool useEnvironmentLighting;
uniform bool useDynamicReflections;

uniform bool usingTerrain;

uniform float deltaTime;
uniform float oceanLevel;

uniform vec3 skyColor;
uniform vec3 cameraPosition;

uniform int numLights;
uniform vec3 lightPositions[LIGHTS_MAX];
uniform vec3 lightColors[LIGHTS_MAX];
uniform float lightBrightness[LIGHTS_MAX];
uniform bool isPointLight[LIGHTS_MAX];

out vec4 outColor;


float calculateAttenuation(float distance) {
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
}

float fresnelSchlickScalar(float cosTheta, float iorRatio) {
    float r0 = (1.0 - iorRatio) / (1.0 + iorRatio);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

vec3 terrainColor(float height) {
    if (isOcean > 0.5) {
        return mix(vec3(0.0, 0.0, 0.4), vec3(0.0, 0.3, 0.8), 0.5);
    }

    // Three segments across four stops (low, mid, mid<->high, high) with
    // smoothstep easing per segment.
    float t = clamp((height - oceanLevel) / 0.5, 0.0, 1.0);
    vec3 stops[4] = vec3[](
        colorLow,
        baseColor,
        mix(baseColor, colorHigh, 0.5),
        colorHigh
    );

    float scaled = t * 3.0;
    int i = min(int(scaled), 2);
    float frac = smoothstep(0.0, 1.0, scaled - float(i));
    return mix(stops[i], stops[i + 1], frac);
}

vec3 gradientColor(float height) {
    float t = clamp(height * 0.8, 0.0, 1.0);
    const float peakStart = 0.7;
    if (t < peakStart)
        return mix(colorLow, baseColor, t / peakStart);
    return mix(baseColor, colorHigh, (t - peakStart) / (1.0 - peakStart));
}


void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    vec3 color = vec3(0.0);
    if(usingTerrain)
    color = terrainColor(vHeight);
    else
    color = gradientColor(vHeight);

    bool waterSurface = isOcean > 0.5;

    // Material surface color
    vec3 surfaceColor = waterSurface ? vec3(0.02, 0.12, 0.22) : color;

    // === AMBIENT + DIRECT LIGHTING
    vec3 ambient = 0.15 * surfaceColor;
    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < numLights && i < LIGHTS_MAX; ++i) {
        vec3 lightDir;
        float attenuation = 1.0;
        float intensity = lightBrightness[i];

        if (isPointLight[i]) {
            vec3 lightVec = lightPositions[i] - fragPosition;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            attenuation = calculateAttenuation(distance);
        } else {
            lightDir = normalize(-lightPositions[i]);
        }

        float diff = max(dot(normal, lightDir), 0.0);
        totalDiffuse += lightColors[i] * (pd * diff) * attenuation * intensity;

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float effectiveNs = waterSurface ? max(ns, 64.0) : ns;
        float spec = effectiveNs < 1.0 ? 1.0
                   : pow(max(dot(normal, halfwayDir), 0.0), effectiveNs);
        totalSpecular += lightColors[i] * (ps * spec) * attenuation * intensity;
    }

    vec3 lighting = ambient + (totalDiffuse * surfaceColor) + totalSpecular;

    // === ENVIRONMENT REFLECTION

    // Water keeps a floor reflectance so it still shines with low slider values
    float reflBase = waterSurface ? max(reflectivity, 0.12) : reflectivity;
    if (reflBase > 0.0) {
        vec3 reflectDir = reflect(-viewDir, normal);
        vec3 envReflection = vec3(0.0);
        if (useDynamicReflections)
            envReflection = texture(envMapSampler, reflectDir).rgb;
        else if (useEnvironmentLighting)
            envReflection = texture(skyboxSampler, reflectDir).rgb;

        float cosTheta = max(dot(normal, viewDir), 0.0);
        float fresnel  = pow(1.0 - cosTheta, 5.0);
        float reflStrength = reflBase * (0.4 + 0.6 * fresnel);
        lighting = mix(lighting, envReflection, reflStrength);
    }

    vec3 finalRgb = lighting;
    float finalAlpha = 1.0;

    // === REFRACTIVE PATH
    if (transparency > 0.0 && ior > 1.0) {
        float cosThetaR = dot(viewDir, normal);
        vec3 n = normal;
        float eta;

        if (cosThetaR > 0.0) {
            eta = 1.0 / ior;
        } else {
            eta = ior;
            n = -normal;
            cosThetaR = -cosThetaR;
        }

        vec3 reflectDir = reflect(-viewDir, n);
        vec3 refractDir = refract(-viewDir, n, eta);
        bool tir = length(refractDir) < 0.001;
        if (tir) refractDir = reflectDir;

        vec3 envRefl = vec3(0.0);
        vec3 envRefr = vec3(0.0);
        if (useDynamicReflections) {
            envRefl = texture(envMapSampler, reflectDir).rgb;
            envRefr = texture(envMapSampler, refractDir).rgb;
        } else if (useEnvironmentLighting) {
            envRefl = texture(skyboxSampler, reflectDir).rgb;
            envRefr = texture(skyboxSampler, refractDir).rgb;
        }

        float fresnelFactor = tir ? 1.0 : fresnelSchlickScalar(cosThetaR, eta);

        vec3 refractedColor  = envRefr * color;
        vec3 transparentCol  = mix(refractedColor, envRefl, fresnelFactor) + totalSpecular;

        finalRgb   = mix(lighting, transparentCol, transparency);
        finalAlpha = mix(1.0, 1.0 - transparency * (1.0 - fresnelFactor), transparency);
    }

    // Apply fog
    finalRgb = mix(skyColor, finalRgb, visibility);

    outColor = vec4(finalRgb, finalAlpha);
}