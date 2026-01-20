#version 330 core

in vec3 texCoords;

out vec4 outColor;
uniform samplerCube skyboxSampler;

void main() {
    outColor = texture(skyboxSampler, texCoords);
}