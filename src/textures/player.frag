#version 330 core

in vec2 fragTexCoord;
in vec3 lightingColor;

uniform sampler2D modelTexture;

out vec4 FragColor;

void main() {
    vec3 texColor = texture(modelTexture, fragTexCoord).rgb;
    FragColor = vec4(texColor * lightingColor, 1.0);
}
