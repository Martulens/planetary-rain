#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float cloudRadius; // scale unit sphere mesh to this radius

out vec3 fragPosition; // world-space position on the cloud shell surface

void main() {
    vec3 pos = normalize(position) * cloudRadius;
    vec4 worldPos = modelMatrix * vec4(pos, 1.0);
    fragPosition = worldPos.xyz;
    gl_Position = projectionMatrix * viewMatrix * worldPos;
}
