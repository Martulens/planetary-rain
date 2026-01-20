#version 330 core

// VERTEX SHADER TEMPLATE
// -> compatible with ShaderProgram class uniform locations

// --- Vertex Attributes (must match MeshGeometry VAO layout) ---
layout(location = 0) in vec3 position;      // Vertex position
layout(location = 1) in vec2 texCoord;      // Texture coordinates
layout(location = 2) in vec3 normal;        // Vertex normal
layout(location = 3) in vec3 color;         // Vertex color (optional)

// --- Transformation Matrices ---
uniform mat4 modelMatrix;                   // Model -> World
uniform mat4 viewMatrix;                    // World -> Camera
uniform mat4 projectionMatrix;              // Camera -> Clip

// --- Fog Parameters ---
uniform float density;                      // Fog density (default: 0.003)
uniform float gradient;                     // Fog gradient (default: 1.5)

// --- Outputs to Fragment Shader ---
out vec3 fragPosition;                      // World-space position
out vec2 fragTexCoord;                      // Texture coordinates
out vec3 fragNormal;                        // World-space normal
out vec3 fragColor;                         // Vertex color
out float visibility;                       // Fog visibility factor (0 = fog, 1 = clear)

void main() {
    // Transform vertex to world space
    vec4 worldPosition = modelMatrix * vec4(position, 1.0);

    // Transform to camera space for fog calculation
    vec4 cameraPosition = viewMatrix * worldPosition;
    float distanceToCamera = length(cameraPosition.xyz);

    // Calculate fog visibility using exponential fog
    // visibility = e^(-(distance * density)^gradient)
    visibility = exp(-pow(distanceToCamera * density, gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    // Pass data to fragment shader
    fragPosition = worldPosition.xyz;
    fragTexCoord = texCoord;
    fragColor = color;

    // Transform normal to world space
    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    fragNormal = normalize(normalMatrix * normal);

    // Final clip-space position
    gl_Position = projectionMatrix * viewMatrix * worldPosition;
}