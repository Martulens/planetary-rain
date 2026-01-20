#version 330 core

#define LIGHTS_MAX 20

// --- Inputs from Vertex Shader ---
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragColor;
in float visibility;

// --- Material Properties ---
uniform vec3 baseColor;
uniform float pd;
uniform float ps;
uniform float ns;
uniform float reflectivity;

// --- Textures ---
uniform sampler2D textureSampler;
uniform samplerCube skyboxSampler;
uniform bool useEnvironmentLighting;

// --- Environment ---
uniform vec3 skyColor;
uniform vec3 cameraPosition;

// --- Lighting ---
uniform int numLights;
uniform vec3 lightPositions[LIGHTS_MAX];
uniform vec3 lightColors[LIGHTS_MAX];
uniform float lightBrightness[LIGHTS_MAX];
uniform bool isPointLight[LIGHTS_MAX];

// --- Output ---
out vec4 outColor;

float calculateAttenuation(float distance) {
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
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

    // === ENVIRONMENT LIGHTING ===
    vec3 ambient;
    vec3 envReflection = vec3(0.0);

    if (useEnvironmentLighting) {
        // Ambient: sample skybox in normal direction (diffuse irradiance approximation)
        ambient = texture(skyboxSampler, normal).rgb * 0.25 * surfaceColor;

        // Reflection: sample skybox in reflected direction
        vec3 reflectDir = reflect(-viewDir, normal);
        envReflection = texture(skyboxSampler, reflectDir).rgb;
    } else {
        ambient = 0.2 * surfaceColor;
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

        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        totalDiffuse += lightColors[i] * (pd * diff) * attenuation * intensity;

        // Specular (Blinn-Phong)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), ns);
        totalSpecular += lightColors[i] * (ps * spec) * attenuation * intensity;
    }

    // === COMBINE ===
    vec3 lighting = ambient + (totalDiffuse * surfaceColor) + totalSpecular;

    // Blend in environment reflection based on reflectivity
    lighting = mix(lighting, envReflection, reflectivity);

    // Apply fog
    vec3 finalColor = mix(skyColor, lighting, visibility);

    outColor = vec4(finalColor, 1.0);
}