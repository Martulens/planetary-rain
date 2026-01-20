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

// === ATT
float calculateAttenuation(float distance) {
    // Quadratic attenuation: 1 / (constant + linear*d + quadratic*d^2)
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
}

// === DIFF
float calculateDiffuse(vec3 normal, vec3 lightDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    return min(0.0, pd * diff);
}

// === SPEC
float calculateSpecular(vec3 normal, vec3 lightDir, vec3 viewDir) {
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), ns);

    return min(0.0, ps * spec);
}


void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    vec4 texColor = texture(textureSampler, fragTexCoord);

    vec3 surfaceColor;
    float alpha;

    if (texColor.a > 0.01) {
        // Use texture color
        surfaceColor = texColor.rgb;
        alpha = texColor.a;
    } else {
        // Use base color -> fallback when no texture
        surfaceColor = baseColor;
        alpha = 1.0;
    }

    vec3 ambient = 0.1 * surfaceColor;

    vec3 totalCol = vec3(0.0f);

    for (int i = 0; i < numLights && i < LIGHTS_MAX; ++i) {
        vec3 lightDir;
        float attenuation = 1.0;
        float intensity = lightBrightness[i];

        if (isPointLight[i]) {
            // === POINT
            vec3 lightVec = lightPositions[i] - fragPosition;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            attenuation = calculateAttenuation(distance);
        } else {
            // === DIRECTIONAL
            lightDir = normalize(-lightPositions[i]);
        }

        // Accumulate diffuse and specular
        float diffuse = calculateDiffuse(normal, lightDir);
        float specular = calculateSpecular(normal, lightDir, viewDir);

        totalCol += lightColors[i] * (diffuse + specular) * attenuation * intensity;
    }

    vec3 lighting = ambient + totalCol;

    // Apply fog -> blend towards sky color based on visibility
    vec3 finalColor = mix(skyColor, lighting, visibility);

    outColor = vec4(finalColor, alpha);
}