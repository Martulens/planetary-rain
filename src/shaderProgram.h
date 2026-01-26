#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <GL/glew.h>

class ShaderProgram {
private:
    GLuint program;
    GLint posLocation;
    GLint colorLocation;

    // Vertex shader uniform locations
    GLint modelMatrix;
    GLint projectionMatrix;
    GLint viewMatrix;

    GLint density;
    GLint gradient;

    // Fragment shader uniform locations
    GLint baseColor;
    GLint ps;
    GLint pd;
    GLint ns;
    GLint reflectivity;
    GLint skyColor;
    GLint ior;
    GLint transparency;

    // Texture sampler uniforms
    GLint backTexture;
    GLint rTexture;
    GLint bTexture;
    GLint gTexture;
    GLint blendMap;

    // Terrain
    GLint usingTerrain;

    GLint screenCoordLocation;

public:
    ShaderProgram() = default;
    ShaderProgram(const std::string& vert, const std::string& frag);
    ~ShaderProgram();

    void loadFloat(GLint location, float value);
    void init(const std::string& vert, const std::string& frag);

    void getSamplerUniforms();
    void getAllUniforms();
    void bindAttributes();

    // === GETTERS
    GLuint getProgram() const{ return program; }
    GLint getPosLocation() const { return posLocation; }
    GLint getColorLocation() const { return colorLocation; }

    GLint getModelMatrixLocation() const { return modelMatrix; }
    GLint getProjectionMatrixLocation() const { return projectionMatrix; }
    GLint getViewMatrixLocation() const { return viewMatrix; }

    GLint getDensityLocation() const { return density; }
    GLint getGradientLocation() const { return gradient; }

    GLint getBaseColorLocation() const { return baseColor; }
    GLint getPsLocation() const { return ps; }
    GLint getPdLocation() const { return pd; }
    GLint getNsLocation() const { return ns; }
    GLint getReflectivityLocation() const { return reflectivity; }
    GLint getSkyColorLocation() const { return skyColor; }
    GLint getIorLocation() const { return ior; }
    GLint getTransparencyLocation() const { return transparency; }

    GLint getBackTextureLocation() const { return backTexture; }
    GLint getRTextureLocation() const { return rTexture; }
    GLint getBTextureLocation() const { return bTexture; }
    GLint getGTextureLocation() const { return gTexture; }

    GLint getUsingTerrain() const { return usingTerrain; };
};


#endif //SHADERPROGRAM_H
