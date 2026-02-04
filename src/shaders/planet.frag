#version 330 core

#define LIGHTS_MAX 20

// === INPUTS FROM VERTEX SHADER ===
in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragColor;
in float visibility;
in float vHeight;

// === UNIFORMS ===
uniform vec3 baseColor;
uniform float pd;           // diffuse coefficient
uniform float ps;           // specular coefficient
uniform float ns;           // shininess exponent

uniform bool usingTerrain;

uniform vec3 skyColor;
uniform vec3 cameraPosition;

uniform int numLights;
uniform vec3 lightPositions[LIGHTS_MAX];
uniform vec3 lightColors[LIGHTS_MAX];
uniform float lightBrightness[LIGHTS_MAX];
uniform bool isPointLight[LIGHTS_MAX];

out vec4 outColor;


vec3 terrainColor(float height) {
    const vec3 colors[6] = vec3[](
    vec3(0.0, 0.0, 0.4),
    vec3(0.0, 0.3, 0.8),
    vec3(0.9, 0.8, 0.6),
    vec3(0.2, 0.6, 0.2),
    vec3(0.5, 0.4, 0.3),
    vec3(1.0, 1.0, 1.0)
    );

    float t = clamp((height - 1.0) * 2.0, 0.0, 1.0);
    float scaled = t * 5.0;
    int i = min(int(scaled), 4);

    return mix(colors[i], colors[i + 1], scaled - float(i));
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

    // === AMBIENT ===
    vec3 ambient = 0.1 * fragColor;

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

        float diff = dot(normal, lightDir);
        diff = max(diff, 0.0);

        vec3 refl = reflect(lightDir, normal);
        float spec = dot(refl, viewDir);
        spec = max(spec, 0.0);

        diffuse += diff * lightBrightness[i] * lightColors[i];
        specular += diff * lightBrightness[i] * lightColors[i];
    }

    // === COMBINE ===

    // no ambient ->
    vec3 result = (pd * diffuse * color) + (ps * specular);

    // ambient ->
    //vec3 result = ambient + (pd * diffuse * color) + (ps * specular);

    // === FOG ===
    // result = mix(skyColor, result, visibility);

    outColor = vec4(result, 1.0);
}