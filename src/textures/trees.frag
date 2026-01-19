#version 330 core
#define MAX_POINT_LIGHTS 20

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in float visibility;

uniform bool isPointLight;

uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float lightBrightness;
uniform vec3 lightColour;
uniform vec3 skyColor;

uniform bool flashlightOn;
uniform vec3 flashlightPos;
uniform vec3 flashlightDir;
uniform float cutoffAngle;

uniform float shineDamper;
uniform float reflectivity;
uniform sampler2D textureSampler;

uniform float time;
uniform int numPointLights;
uniform vec3 pointLightPositions[MAX_POINT_LIGHTS];
uniform vec3 pointLightColors[MAX_POINT_LIGHTS];

out vec4 fragColor;

vec3 calculateLight(vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos, vec3 lightColor, float brightness, bool isPoint) {
    vec3 lightDir = isPoint ? normalize(lightPos - fragPos) : normalize(-lightPos);  // directional or point
    vec3 ambient = 0.15 * lightColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * vec3(1.0);

    return (ambient + diffuse + specular) * brightness;
}

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(-fragPosition);

    // Total color
    vec3 lightingColor = vec3(0.0);

    // Sun
    lightingColor += calculateLight(norm, fragPosition, viewDir, lightDirection, lightColour, lightBrightness, false);

    // Moon (point light)
    if (isPointLight) {
        lightingColor += calculateLight(norm, fragPosition, viewDir, lightPosition, lightColour, lightBrightness, true);
    }

    if (flashlightOn) {
        vec3 flashDir = normalize(flashlightDir);
        vec3 lightToFrag = normalize(fragPosition - flashlightPos); // this direction
        float theta = dot(lightToFrag, normalize(flashlightDir));   // compare with spotlight direction

        float epsilon = 0.1;
        float intensity = clamp((theta - cos(cutoffAngle)) / epsilon, 0.0, 1.0);

        lightingColor += intensity * 7.5 * calculateLight(norm, fragPosition, viewDir, flashlightPos, vec3(1.0), 1.0, true);
    }

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


    // Sample texture
    vec4 textureColor = texture(textureSampler, fragTexCoord);
    if (isPointLight) 
        textureColor *= 0.6;

    // Subtle nighttime blue tint
    vec3 nightTint = vec3(0.3, 0.35, 0.5);
    vec3 finalColor = isPointLight ? lightingColor * 0.5 + nightTint * 0.5 : lightingColor;
    finalColor *= textureColor.rgb;

    // Fog fade
    finalColor = mix(skyColor, finalColor, visibility);
    fragColor = vec4(finalColor, textureColor.a);
}
