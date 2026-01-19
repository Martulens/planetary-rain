#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;

uniform vec3 lightColour;
uniform vec3 fakeLightDir;
uniform bool isMoon;
uniform float time;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(-fragPosition);
    vec3 lightDir = normalize(-fakeLightDir);

    // Half-Lambert diffuse
    float diff = dot(norm, lightDir);
    diff = diff * 0.5 + 0.5;
    vec3 diffuse = lightColour * diff;

    vec3 ambient = lightColour * 0.3;
    vec3 emissive = lightColour * 0.05;

    // Rim lighting
    float rimFactor = 1.0 - max(dot(norm, viewDir), 0.0);
    rimFactor = smoothstep(0.0, 1.0, rimFactor);
    rimFactor = pow(rimFactor, 2.5);
    float pulse = 0.5 + 0.5 * sin(time * 2.0);
    vec3 rimColor = vec3(0.7, 0.8, 1.0);

    vec3 finalColor = ambient + diffuse + emissive;
    if (isMoon)
        finalColor += rimColor * rimFactor * 0.4 * pulse;

    // Fade glow when close
    float dist = length(fragPosition);
    float fade = smoothstep(5.0, 50.0, dist);

    fragColor = vec4(clamp(finalColor, 0.0, 1.0) * fade, 1.0);
}