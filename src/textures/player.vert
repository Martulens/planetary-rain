#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 color;
layout (location = 4) in ivec4 boneIDs;
layout (location = 5) in vec4 weights;

uniform mat4 transformationMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 bones[100];

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

uniform float density;
uniform float gradient;

out vec2 fragTexCoord;
out vec3 lightingColor;

void main() {
    // Skinning transformation
    mat4 boneTransform = 
        weights[0] * bones[boneIDs[0]] +
        weights[1] * bones[boneIDs[1]] +
        weights[2] * bones[boneIDs[2]] +
        weights[3] * bones[boneIDs[3]];

    vec4 skinnedPosition = boneTransform * vec4(position, 1.0);
    vec3 skinnedNormal = mat3(boneTransform) * normal;

    // World and clip-space
    vec4 worldPos = transformationMatrix * skinnedPosition;
    vec4 viewPos = viewMatrix * worldPos;
    gl_Position = projectionMatrix * viewPos;

    vec3 fragPos = worldPos.xyz;
    vec3 worldNormal = normalize(mat3(transformationMatrix) * normal);

    vec3 lightDir = normalize(lightPosition - fragPos);
    vec3 viewDir = normalize(-fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(worldNormal, lightDir), 0.2);
    float spec = pow(max(dot(worldNormal, halfwayDir), 0.0), 32.0);
    float specStrength = 0.5;

    vec3 tint = (isPointLight) ? vec3(0.7, 0.8, 1.0) : vec3(1.0);
    vec3 light = lightColour * tint;

    lightingColor = diff * light + specStrength * spec * light;
    fragTexCoord = texCoord;
}
