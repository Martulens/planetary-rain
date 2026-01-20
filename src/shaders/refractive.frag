#version 330 core

#define LIGHTS_MAX 20

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragColor;
in float visibility;

uniform vec3 baseColor;
uniform float pd;
uniform float ps;
uniform float ns;
uniform float reflectivity;

uniform sampler2D textureSampler;
uniform samplerCube skyboxSampler;
uniform samplerCube envMapSampler;
uniform bool useEnvironmentLighting;
uniform bool useDynamicReflections;

uniform vec3 skyColor;
uniform vec3 cameraPosition;

uniform int numLights;
uniform vec3 lightPositions[LIGHTS_MAX];
uniform vec3 lightColors[LIGHTS_MAX];
uniform float lightBrightness[LIGHTS_MAX];
uniform bool isPointLight[LIGHTS_MAX];

// === REFRACTIVE UNIFORMS ===
uniform float ior;           // Index of refraction (glass ~1.5, water ~1.33, diamond ~2.4)
uniform float transparency;  // 0.0 = opaque, 1.0 = fully transparent

out vec4 outColor;

float calculateAttenuation(float distance) {
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
}

// Fresnel-Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Schlick's approximation for Fresnel reflectance (scalar version for dielectrics)
float fresnelSchlickScalar(float cosTheta, float iorRatio) {
    float r0 = (1.0 - iorRatio) / (1.0 + iorRatio);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    // Determine if we're entering or exiting the material
    float cosTheta = dot(viewDir, normal);
    vec3 n = normal;
    float eta;  // ratio of indices of refraction

    if (cosTheta > 0.0) {
        // Ray entering the material (air -> material)
        eta = 1.0 / ior;
    } else {
        // Ray exiting the material (material -> air)
        eta = ior;
        n = -normal;
        cosTheta = -cosTheta;
    }

    // Surface color
    vec3 surfaceColor = baseColor;
    vec4 texColor = texture(textureSampler, fragTexCoord);
    if (texColor.r > 0.01 || texColor.g > 0.01 || texColor.b > 0.01) {
        surfaceColor = texColor.rgb;
    }

    // === REFLECTION ===
    vec3 reflectDir = reflect(-viewDir, n);

    // === REFRACTION ===
    vec3 refractDir = refract(-viewDir, n, eta);

    // Total internal reflection check
    bool totalInternalReflection = (length(refractDir) < 0.001);
    if (totalInternalReflection) {
        refractDir = reflectDir;  // Fall back to reflection
    }

    // Sample environment maps
    vec3 envReflection = vec3(0.0);
    vec3 envRefraction = vec3(0.0);

    if (useDynamicReflections) {
        envReflection = texture(envMapSampler, reflectDir).rgb;
        envRefraction = texture(envMapSampler, refractDir).rgb;
    } else if (useEnvironmentLighting) {
        envReflection = texture(skyboxSampler, reflectDir).rgb;
        envRefraction = texture(skyboxSampler, refractDir).rgb;
    }

    // === FRESNEL - determines reflection vs refraction ratio ===
    float fresnelFactor = fresnelSchlickScalar(cosTheta, eta);

    // Total internal reflection means 100% reflection
    if (totalInternalReflection) {
        fresnelFactor = 1.0;
    }

    // === AMBIENT ===
    vec3 ambient;
    if (useEnvironmentLighting || useDynamicReflections) {
        vec3 envAmbient = useDynamicReflections
        ? texture(envMapSampler, normal).rgb
        : texture(skyboxSampler, normal).rgb;
        ambient = envAmbient * 0.1 * surfaceColor;
    } else {
        ambient = 0.1 * surfaceColor;
    }

    // === DIRECT LIGHTING (specular highlights on glass) ===
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < numLights && i < LIGHTS_MAX; ++i) {
        vec3 lightDir;
        float attenuation = 1.0;
        float intensity = lightBrightness[i];

        if (isPointLight[i]) {
            vec3 lightVec = lightPositions[i] - fragPosition;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            attenuation = calculateAttenuation(distance);
        } else {
            lightDir = normalize(-lightPositions[i]);
        }

        // Specular only for transparent materials (no diffuse - light passes through)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(n, halfwayDir), 0.0), ns);
        totalSpecular += lightColors[i] * (ps * spec) * attenuation * intensity;
    }

    vec3 reflectedColor = envReflection;
    vec3 refractedColor = envRefraction * surfaceColor;

    vec3 transparentColor = mix(refractedColor, reflectedColor, fresnelFactor);

    // Add specular highlights on top
    transparentColor += totalSpecular;

    // === BLEND WITH OPAQUE LIGHTING ===
    vec3 opaqueColor = ambient + surfaceColor * 0.3;

    vec3 finalColor = mix(opaqueColor, transparentColor, transparency);

    // Apply fog
    finalColor = mix(skyColor, finalColor, visibility);

    // Alpha based on transparency and Fresnel
    float alpha = mix(1.0, 1.0 - transparency * (1.0 - fresnelFactor), transparency);

    outColor = vec4(finalColor, alpha);
}