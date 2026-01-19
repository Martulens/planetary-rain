#version 330 core

in vec3 texCoord;
out vec4 fragColor;

uniform samplerCube skybox;
uniform float skyboxBlend; // 0 = night, 1 = day

void main() {
    vec4 dayColor = texture(skybox, normalize(texCoord));
    vec4 nightColor = vec4(0.02, 0.02, 0.08, 1.0); // Night sky color
    fragColor = mix(nightColor, dayColor, skyboxBlend);
}
