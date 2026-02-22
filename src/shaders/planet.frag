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
uniform vec3 baseColor;
uniform float pd;           // diffuse coefficient
uniform float ps;           // specular coefficient
uniform float ns;           // shininess exponent

// -> reflection
uniform float reflectivity;
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

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 terrainColor(float height) {
    const vec3 colors[6] = vec3[](
    vec3(0.0, 0.0, 0.4),
    vec3(0.0, 0.3, 0.8),
    vec3(0.9, 0.8, 0.6),
    vec3(0.2, 0.6, 0.2),
    vec3(0.5, 0.4, 0.3),
    vec3(1.0, 1.0, 1.0)
    );

    if (isOcean > 0.5) {
        return mix(colors[0], colors[1], 0.5);
    }

    float t = clamp((height - oceanLevel) / 0.5, 0.0, 1.0);
    float scaled = t * 3.0;
    int i = min(int(scaled), 2);

    return mix(colors[i + 2], colors[i + 3], scaled - float(i));
}

vec3 gradientColor(float height) {
    vec3 baseColor = fragColor;
    vec3 endColor = fragColor + vec3(0.5f);

    return mix(baseColor, endColor, height - 1);
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

    // === REFLECTION
    vec3 envReflection = vec3(0.0);
    vec3 fresnel = vec3(0.0);

    if(waterSurface){
        vec3 reflectDir = reflect(-viewDir, normal);

        if (useDynamicReflections) {
            envReflection = texture(envMapSampler, reflectDir).rgb;
        } else if (useEnvironmentLighting) {
            envReflection = texture(skyboxSampler, reflectDir).rgb;
        }

        float cosTheta = max(dot(normal, viewDir), 0.0);
        // -> water has low base reflectance
        vec3 F0 = vec3(0.2);
        fresnel = fresnelSchlick(cosTheta, F0);
    }

    // === AMBIENT ===
    vec3 ambient = 0.15 * color;

    // === DIRECT LIGHTING ===
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
        float waterNs = waterSurface ? max(ns, 64.0) : ns;
        float spec = pow(max(dot(normal, halfwayDir), 0.0), waterNs);

        if(waterNs < 1.0)
        spec = 1.0;

        totalSpecular += lightColors[i] * (ps * spec) * attenuation * intensity;
    }

    // === COMBINE ===
    vec3 lighting = ambient + (totalDiffuse * color) + totalSpecular;

    // Blend in reflections for water
    if(waterSurface){
        vec3 waterBase = vec3(0.02, 0.12, 0.22);
        float fresnelFactor = (fresnel.r + fresnel.g + fresnel.b) / 3.0;

        vec3 waterLit = ambient + (totalDiffuse * waterBase) + totalSpecular;
        lighting = waterLit;
    }

    // Apply fog
    vec3 finalColor = mix(skyColor, lighting, visibility);

    outColor = vec4(finalColor, 1.0);
}