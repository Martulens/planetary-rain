#version 330 core

#define LIGHTS_MAX 20

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragColor;
in float visibility;

uniform vec3 baseColor;
uniform float pd;
uniform float ps;
uniform float ns;
uniform float reflectivity;

uniform sampler2D textureSampler;
uniform samplerCube skyboxSampler;
uniform samplerCube envMapSampler;
uniform bool useEnvironmentLighting;
uniform bool useDynamicReflections;

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

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    // Surface color
    vec3 surfaceColor = baseColor;
    vec4 texColor = texture(textureSampler, fragTexCoord);
    if (texColor.r > 0.01 || texColor.g > 0.01 || texColor.b > 0.01) {
        surfaceColor = texColor.rgb;
    }

    // === REFLECTION ===
    vec3 reflectDir = reflect(-viewDir, normal);
    vec3 envReflection = vec3(0.0);

    if (useDynamicReflections) {
        // Use dynamic environment map (reflects other objects)
        envReflection = texture(envMapSampler, reflectDir).rgb;
    } else if (useEnvironmentLighting) {
        // Fall back to skybox reflection
        envReflection = texture(skyboxSampler, reflectDir).rgb;
    }

    // === FRESNEL ===
    float cosTheta = max(dot(normal, viewDir), 0.0);
    vec3 F0 = vec3(0.04);  // Base reflectivity for dielectrics
    F0 = mix(F0, surfaceColor, reflectivity);  // Metallic surfaces reflect their color
    vec3 fresnel = fresnelSchlick(cosTheta, F0);

    // === AMBIENT ===
    vec3 ambient;
    if (useEnvironmentLighting || useDynamicReflections) {
        // Sample environment in normal direction for ambient
        vec3 envAmbient = useDynamicReflections
        ? texture(envMapSampler, normal).rgb
        : texture(skyboxSampler, normal).rgb;
        ambient = envAmbient * 0.2 * surfaceColor;
    } else {
        ambient = 0.15 * surfaceColor;
    }

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
        float spec = pow(max(dot(normal, halfwayDir), 0.0), ns);
        totalSpecular += lightColors[i] * (ps * spec) * attenuation * intensity;
    }

    // === COMBINE ===
    vec3 lighting = ambient + (totalDiffuse * surfaceColor) + totalSpecular;

    // Blend reflection using Fresnel
    float reflectionStrength = reflectivity * fresnel.r;
    lighting = mix(lighting, envReflection, reflectionStrength);

    // Apply fog
    vec3 finalColor = mix(skyColor, lighting, visibility);

    outColor = vec4(finalColor, 1.0);
}