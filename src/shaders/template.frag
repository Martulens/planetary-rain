#version 330 core

// FRAGMENT SHADER TEMPLATE
// Features:
// -> Blinn-Phong lighting from multiple lights
// -> fog
// -> textures

#define LIGHTS_MAX 20

// --- Inputs from Vertex Shader ---
in vec3 fragPosition;                       // World-space position
in vec2 fragTexCoord;                       // Texture coordinates
in vec3 fragNormal;                         // World-space normal
in vec3 fragColor;                          // Vertex color
in float visibility;                        // Fog visibility (0 = fog, 1 = clear)

// --- Material Properties ---
uniform vec3 baseColor;                     // Base color when no texture
uniform float pd;                           // Diffuse coefficient (0-1)
uniform float ps;                           // Specular coefficient (0-1)
uniform float ns;                           // Specular exponent/shininess (1-256)
uniform float reflectivity;                 // Reflectivity (for future use)

// --- Textures ---
uniform sampler2D textureSampler;           // Main diffuse texture

// --- Environment ---
uniform vec3 skyColor;                      // Sky/fog color
uniform vec3 cameraPosition;                // Camera world position

// --- Lighting ---
uniform float time;                         // Elapsed time for effects
uniform int numLights;                      // Number of active lights
uniform vec3 lightPositions[LIGHTS_MAX];    // Light positions (or directions for directional)
uniform vec3 lightColors[LIGHTS_MAX];       // Light colors
uniform float lightBrightness[LIGHTS_MAX];  // Light intensity multipliers
uniform bool isPointLight[LIGHTS_MAX];      // true = point light, false = directional

// --- Output ---
out vec4 outColor;

// === ATTENUATION
float calculateAttenuation(float distance) {
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
}

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    // Determine surface color: use baseColor directly
    // baseColor is set from ModelTexture, which has the color we want
    vec3 surfaceColor = baseColor;

    // If we have a valid texture, blend or replace
    vec4 texColor = texture(textureSampler, fragTexCoord);
    // Check if texture has meaningful data (not default black with full alpha)
    if (texColor.r > 0.01 || texColor.g > 0.01 || texColor.b > 0.01) {
        surfaceColor = texColor.rgb;
    }

    // Ambient component
    vec3 ambient = 0.2 * surfaceColor;

    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < numLights && i < LIGHTS_MAX; ++i) {
        vec3 lightDir;
        float attenuation = 1.0;
        float intensity = lightBrightness[i];

        if (isPointLight[i]) {
            // === POINT LIGHT
            vec3 lightVec = lightPositions[i] - fragPosition;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            attenuation = calculateAttenuation(distance);
        } else {
            // === DIRECTIONAL LIGHT
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

    // Combine lighting
    vec3 lighting = ambient + (totalDiffuse * surfaceColor) + totalSpecular;

    // Apply fog
    vec3 finalColor = mix(skyColor, lighting, visibility);

    outColor = vec4(finalColor, 1.0);
}