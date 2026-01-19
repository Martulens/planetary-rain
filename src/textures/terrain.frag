#version 330 core
#define MAX_POINT_LIGHTS 20

in vec3 fragPosition;
in vec2 fragTexCoord;
flat in vec3 fragNormal;
in float visibility;

uniform bool isPointLight;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float lightBrightness;
uniform vec3 lightColour;
uniform vec3 skyColor;

uniform float shineDamper;
uniform float reflectivity;

uniform bool flashlightOn;
uniform vec3 flashlightPos;
uniform vec3 flashlightDir;
uniform float cutoffAngle;

// 0
uniform sampler2D backB, backN, backR, backH, backE;
uniform sampler2D redB,  redN,  redR,  redH,  redE;
uniform sampler2D greenB, greenN, greenR, greenH, greenE;
uniform sampler2D blueB,  blueN,  blueR,  blueH,  blueE;

uniform sampler2D blendMap;

uniform float time;
uniform int numPointLights;
uniform vec3 pointLightPositions[MAX_POINT_LIGHTS];
uniform vec3 pointLightColors[MAX_POINT_LIGHTS];

out vec4 fragColor;

vec3 calculateLight(vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos, vec3 lightColor, float brightness, bool isPoint, float shininess) {
    vec3 lightDir = isPoint ? normalize(lightPos - fragPos) : normalize(-lightPos);
    vec3 ambient = 0.15 * lightColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = spec * vec3(1.0);

    return (ambient + diffuse + specular) * brightness;
}

void main() {
    vec2 tiledCoords = fragTexCoord * 50.0;
    vec2 waterOffset = vec2(time * 0.06, sin(time * 0.8) * 0.3);
    vec2 waterCoords = tiledCoords + waterOffset;

    vec4 blend = texture(blendMap, fragTexCoord);
    float backWeight = 1.0 - (blend.r + blend.g + blend.b);

    // base
    vec3 baseColor =
        texture(backB,  tiledCoords).rgb * backWeight +
        texture(redB,   tiledCoords).rgb * blend.r +
        texture(greenB, waterCoords).rgb * blend.g +
        texture(blueB,  tiledCoords).rgb * blend.b;

    if (isPointLight) 
        baseColor *= 0.3; 
    
    // normal
    vec3 nBack  = texture(backN,  tiledCoords).rgb * 2.0 - 1.0;
    vec3 nRed   = texture(redN,   tiledCoords).rgb * 2.0 - 1.0;
    vec3 nGreen = texture(greenN, waterCoords).rgb * 2.0 - 1.0;
    vec3 nBlue  = texture(blueN,  tiledCoords).rgb * 2.0 - 1.0;

    vec3 blendedNormal = normalize(
        nBack  * backWeight +
        nRed   * blend.r +
        nGreen * blend.g +
        nBlue  * blend.b
    );

    // roughness
    float roughness =
        texture(backR,  tiledCoords).r * backWeight +
        texture(redR,   tiledCoords).r * blend.r +
        texture(greenR, waterCoords).r * blend.g +
        texture(blueR,  tiledCoords).r * blend.b;

    // height
    float height =
        texture(backH,  tiledCoords).r * backWeight +
        texture(redH,   tiledCoords).r * blend.r +
        texture(greenH, waterCoords).r * blend.g +
        texture(blueH,  tiledCoords).r * blend.b;

    // emissive
    vec3 emissive =
        texture(backE,  tiledCoords).rgb * backWeight +
        texture(redE,   tiledCoords).rgb * blend.r +
        texture(greenE, waterCoords).rgb * blend.g +
        texture(blueE,  tiledCoords).rgb * blend.b;

    vec3 norm = normalize(blendedNormal * fragNormal);
    vec3 viewDir = normalize(-fragPosition);

    // Total color
    vec3 lightingColor = vec3(0.0);
    float shininess = mix(8.0, 128.0, 1.0 - roughness);

    // Light
    lightingColor = calculateLight(norm, fragPosition, viewDir, lightDirection, lightColour, lightBrightness, false, shininess);

    if (isPointLight) {
        lightingColor += calculateLight(norm, fragPosition, viewDir, lightPosition, lightColour, lightBrightness, true, shininess);
    }

    // Flashlight
    if (flashlightOn) {
        vec3 lightToFrag = normalize(fragPosition - flashlightPos);
        float theta = dot(lightToFrag, normalize(flashlightDir));
        float epsilon = 0.1;
        float intensity = clamp((theta - cos(cutoffAngle)) / epsilon, 0.0, 1.0);

        lightingColor += intensity * 7.5 * calculateLight(norm, fragPosition, viewDir, flashlightPos, vec3(1.0), 1.0, true, shininess);
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

    // Night-time tint
    vec3 nightTint = vec3(0.3, 0.35, 0.5);
    vec3 finalLight = isPointLight ? lightingColor * 0.25 + nightTint * 0.75 : lightingColor;

    // Final color
    vec3 resultColor = baseColor * (finalLight + emissive);

    float heightFactor = clamp(1.0 - height * 0.005, 0.75, 1.0);
    resultColor *= heightFactor;

    // Fog
    resultColor = mix(skyColor, resultColor, visibility);

    fragColor = vec4(resultColor, 1.0);
}
