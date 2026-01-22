#version 330 core

#define LIGHTS_MAX 20

// === INPUTS FROM VERTEX SHADER ===
in vec3 fragPosition;
in vec3 fragNormal;
in float visibility;

// === UNIFORMS ===
uniform vec3 baseColor;
uniform float pd;           // diffuse coefficient
uniform float ps;           // specular coefficient
uniform float ns;           // shininess exponent

uniform vec3 skyColor;
uniform vec3 cameraPosition;

uniform int numLights;
uniform vec3 lightPositions[LIGHTS_MAX];
uniform vec3 lightColors[LIGHTS_MAX];
uniform float lightBrightness[LIGHTS_MAX];
uniform bool isPointLight[LIGHTS_MAX];

out vec4 outColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    // === AMBIENT ===
    vec3 ambient = 0.1 * baseColor;

    // === ACCUMULATE LIGHTING ===
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < numLights && i < LIGHTS_MAX; ++i) {
        vec3 lightDir;
        float attenuation = 1.0;

        if (isPointLight[i]) {
            // Point light: direction from fragment to light
            vec3 lightVec = lightPositions[i] - fragPosition;
            float dist = length(lightVec);
            lightDir = normalize(lightVec);
            attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        } else {
            // Directional light: position is direction (inverted)
            lightDir = normalize(-lightPositions[i]);
        }

        // Diffuse (Lambert)
        float diff = max(dot(normal, lightDir), 0.0);
        diffuse += lightColors[i] * lightBrightness[i] * diff * attenuation;

        // Specular (Blinn-Phong)
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfDir), 0.0), ns);
        specular += lightColors[i] * lightBrightness[i] * spec * attenuation;
    }

    // === COMBINE ===
    float ao = clamp(dot(normal, vec3(0.0, 1.0, 0.0)) * 0.5 + 0.5, 0.3, 1.0);
    ambient *= ao;
    vec3 result = ambient + (pd * diffuse * baseColor) + (ps * specular);

    // === FOG ===
    result = mix(skyColor, result, visibility);

    outColor = vec4(result, 1.0);
}