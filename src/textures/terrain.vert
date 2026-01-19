#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 transformationMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float density;
uniform float gradient;

out vec3 fragPosition;
out vec2 fragTexCoord;
flat out vec3 fragNormal;
out float visibility;

void main() {
    vec4 worldPosition = transformationMatrix * vec4(position, 1.0);
    
    vec4 posToCam = viewMatrix * worldPosition; 
    float camDistance = length(posToCam.xyz);
    visibility = exp(-pow((camDistance*density), gradient));
    visibility = clamp(visibility, 0.0f, 1.0f);

    fragPosition = worldPosition.xyz;
    fragTexCoord = texCoord;

    fragNormal = normalize(mat3(transformationMatrix) * normal);

    gl_Position = projectionMatrix * viewMatrix * worldPosition;
}
