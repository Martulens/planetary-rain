#version 330 core
#define LIGHTS_MAX 20

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in float visibility;

uniform vec3 skyColor;

// === MATERIAL INFO
uniform float pd;
uniform float ps;
uniform float ns;
uniform float reflectivity;
uniform sampler2D textureSampler;

// === LIGHTS
uniform float time;
uniform int numLights;
uniform vec3 lightPositions[LIGHTS_MAX];
uniform vec3 lightColors[LIGHTS_MAX];
uniform float lightBrightness[LIGHTS_MAX];
uniform bool isPointLight[LIGHTS_MAX];

out vec4 fragColor;

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(-fragPosition);

    // Total color
    vec3 lightingColor = vec3(0.0);

    for (int i = 0; i < numPointLights; ++i) {
        vec3 lightDir = pointLightPositions[i] - fragPosition;
        float dist = length(lightDir);
        lightDir = normalize(lightDir);

        // Brighter and bigger torch light
        float attenuation = 1.0 / (0.05 + 0.05 * dist + 0.02 * dist * dist);
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 baseColor = pointLightColors[i];

        // Glowing core intensity multiplier
        float glow = pow(clamp(1.0 - dist / 18.0, 0.0, 1.0), 2.0);  // softer fade, longer reach
        vec3 intenseColor = baseColor * (2.0 + 6.0 * glow);

        float flicker = 0.95 + 0.05 * sin(time * 12.0 + float(i) * 11.0);
        lightingColor += intenseColor * diff * attenuation * flicker;
    }

    // Fog fade
    finalColor = mix(skyColor, finalColor, visibility);
    fragColor = vec4(finalColor, textureColor.a);
}