#version 330 core

layout(location = 0) in vec2 screenPos;

out vec3 texCoord;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    vec4 pos = vec4(screenPos.xy, 1.0, 1.0);
    vec4 worldPos = inverse(projectionMatrix * viewMatrix) * pos;
    texCoord = worldPos.xyz;
    gl_Position = vec4(screenPos, 0.999, 1.0); // Push to far plane
}
