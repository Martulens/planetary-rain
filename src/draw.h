#ifndef DRAW_H
#define DRAW_H

#include "glm/vec3.hpp"
#include <vector>

// Forward declarations
class Object;
class Camera;
class Light;
struct GameState;

// Main drawing functions
void drawWindow();
void drawScene();

// Template drawing functions
void setupUniforms(const Object* obj, const std::vector<Light*>& lights, const Camera* cam, const GameState& gameState);
void setupTextures(const Object* obj);
void drawTemplate(const Object* obj, const Camera* cam, const std::vector<Light*>& lights, const GameState& gameState);

#endif // DRAW_H