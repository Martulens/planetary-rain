#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 transformationMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragNormal;
out vec3 fragPosition;

void main() {
    vec4 worldPos = transformationMatrix * vec4(position, 1.0);
    fragPosition = vec3(worldPos);
    fragNormal = mat3(transpose(inverse(transformationMatrix))) * normal;

    gl_Position = projectionMatrix * viewMatrix * worldPos;
}