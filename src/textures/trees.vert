#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 color;

// Instance matrix
layout (location = 4) in vec4 instanceMatrix0;
layout (location = 5) in vec4 instanceMatrix1;
layout (location = 6) in vec4 instanceMatrix2;
layout (location = 7) in vec4 instanceMatrix3;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightDirection;

uniform float density;
uniform float gradient;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out float visibility;

void main()
{
    mat4 modelMatrix = mat4(instanceMatrix0, instanceMatrix1, instanceMatrix2, instanceMatrix3);
    vec4 worldPosition = modelMatrix * vec4(position, 1.0);

    vec4 posToCam = viewMatrix * worldPosition; 
    float camDistance = length(posToCam.xyz);
    visibility = exp(-pow((camDistance*density), gradient));
    visibility = clamp(visibility, 0.0f, 1.0f);

    fragPosition = vec3(worldPosition);

    fragNormal = mat3(transpose(inverse(modelMatrix))) * normal;
    fragTexCoord = texCoord;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
